// Host tests for the portable logic. These run with a plain host compiler and
// assert behavior; they do NOT touch hardware, I2S, I2C, GPIO, or any network.
//
// Status: this file is "implemented" and the results are "compiled/simulated".
// It is never evidence of hardware-tested behavior.

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "jalak/audio.h"
#include "jalak/expression.h"
#include "jalak/framebuffer.h"
#include "jalak/state_machine.h"

namespace {

int g_failures = 0;

#define CHECK(cond)                                                          \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::printf("FAIL: %s (line %d)\n", #cond, __LINE__);            \
            ++g_failures;                                                    \
        }                                                                    \
    } while (0)

void test_expression_clamp() {
    jalak::PanelGeometry geo{128, 64};
    auto e = jalak::expression_for(jalak::Mood::Surprised);
    CHECK(e.left_aperture_pct > 100);  // raw value intentionally over
    auto c = jalak::clamp_expression(e, geo);
    CHECK(c.left_aperture_pct == 100);
    CHECK(c.right_aperture_pct == 100);

    jalak::Expression wild;
    wild.look_dx = 9999;
    wild.brow_tilt = -999;
    auto w = jalak::clamp_expression(wild, geo);
    CHECK(w.look_dx <= geo.width / 4);
    CHECK(w.brow_tilt >= -10);
}

void test_framebuffer_page_bytes() {
    jalak::PanelGeometry geo{128, 64};
    jalak::Framebuffer fb(geo);
    CHECK(fb.to_page_bytes().size() == 128u * 8u);  // 64/8 = 8 pages

    fb.clear(false);
    fb.set(0, 0, true);
    auto bytes = fb.to_page_bytes();
    CHECK((bytes[0] & 0x01) == 0x01);  // top row is bit 0 of first byte

    fb.clear(false);
    fb.set(0, 7, true);
    bytes = fb.to_page_bytes();
    CHECK((bytes[0] & 0x80) == 0x80);

    // Out-of-bounds writes must be ignored, not crash.
    fb.set(-5, -5, true);
    fb.set(1000, 1000, true);
    CHECK(!fb.get(-5, -5));
}

void test_draw_face_nonempty_and_bounded() {
    jalak::Framebuffer fb({128, 64});
    const int painted = jalak::draw_face(fb, jalak::expression_for(jalak::Mood::Happy));
    CHECK(painted > 0);
    // Nothing may be painted outside the panel (get() returns false outside).
    // Count total painted pixels equals reported count.
    int count = 0;
    for (int y = 0; y < 64; ++y)
        for (int x = 0; x < 128; ++x)
            if (fb.get(x, y)) ++count;
    CHECK(count == painted);
}

void test_state_machine_happy_path() {
    jalak::StateMachine sm;
    CHECK(sm.state() == jalak::State::Boot);
    CHECK(!sm.handle(jalak::Event::StartCapture).accepted);  // not ready yet

    CHECK(sm.handle(jalak::Event::BootComplete).accepted);
    CHECK(sm.state() == jalak::State::Ready);

    CHECK(sm.handle(jalak::Event::StartCapture).accepted);
    CHECK(sm.state() == jalak::State::Listening);
    CHECK(!sm.mic_muted());  // mic live only while listening

    CHECK(sm.handle(jalak::Event::StopCapture).accepted);
    CHECK(sm.state() == jalak::State::Uploading);
    CHECK(sm.mic_muted());

    CHECK(sm.handle(jalak::Event::UploadComplete).accepted);
    CHECK(sm.handle(jalak::Event::TurnSucceeded).accepted);
    CHECK(sm.state() == jalak::State::Speaking);
    CHECK(sm.turn_count() == 1);
    CHECK(sm.mic_muted());  // half-duplex: muted while speaking

    CHECK(sm.handle(jalak::Event::SpeechFinished).accepted);
    CHECK(sm.state() == jalak::State::Ready);
}

void test_state_machine_stop_and_reset() {
    jalak::StateMachine sm;
    sm.handle(jalak::Event::BootComplete);
    sm.handle(jalak::Event::StartCapture);
    CHECK(sm.state() == jalak::State::Listening);

    CHECK(sm.handle(jalak::Event::Stop).accepted);
    CHECK(sm.state() == jalak::State::Stopped);
    CHECK(sm.mic_muted());
    // Stop from stopped is rejected.
    CHECK(!sm.handle(jalak::Event::Stop).accepted);

    CHECK(sm.handle(jalak::Event::Reset).accepted);
    CHECK(sm.state() == jalak::State::Ready);
}

void test_capture_cap_and_session_cap() {
    jalak::StateMachine sm;
    sm.handle(jalak::Event::BootComplete);
    sm.handle(jalak::Event::StartCapture);

    // 8 s cap: tick to 8000 ms triggers StopCapture automatically.
    auto e = sm.tick(7999);
    CHECK(!e.has_value());
    CHECK(sm.state() == jalak::State::Listening);
    e = sm.tick(1);
    CHECK(e.has_value() && *e == jalak::Event::StopCapture);
    CHECK(sm.state() == jalak::State::Uploading);

    // Session cap: new machine, burn the 180 s session without finishing turns.
    jalak::StateMachine s2;
    s2.handle(jalak::Event::BootComplete);
    auto done = s2.tick(180001);
    CHECK(done.has_value() && *done == jalak::Event::Stop);
    CHECK(s2.state() == jalak::State::Stopped);
}

void test_turn_cap() {
    jalak::StateConfig cfg;
    cfg.max_turns = 1;
    jalak::StateMachine sm(cfg);
    sm.handle(jalak::Event::BootComplete);
    sm.handle(jalak::Event::StartCapture);
    sm.handle(jalak::Event::StopCapture);
    sm.handle(jalak::Event::UploadComplete);
    sm.handle(jalak::Event::TurnSucceeded);
    sm.handle(jalak::Event::SpeechFinished);
    CHECK(sm.turn_count() == 1);
    // Second turn must be refused.
    CHECK(!sm.handle(jalak::Event::StartCapture).accepted);
}

void test_book_handoff_is_quiet() {
    jalak::StateMachine sm;
    sm.handle(jalak::Event::BootComplete);
    CHECK(sm.handle(jalak::Event::HandoffToBook).accepted);
    CHECK(sm.state() == jalak::State::BookHandoff);
    CHECK(sm.mic_muted());
    // The robot must not solicit more talk during handoff.
    CHECK(!sm.handle(jalak::Event::StartCapture).accepted);
}

void test_gesture_classification() {
    using jalak::Gesture;
    CHECK(jalak::classify_press(50) == Gesture::Tap);
    CHECK(jalak::classify_press(300) == Gesture::Tap);
    CHECK(jalak::classify_press(500) == Gesture::None);
    CHECK(jalak::classify_press(1000) == Gesture::LongPress);
    CHECK(jalak::classify_press(-1) == Gesture::None);
}

void test_audio_limits() {
    CHECK(jalak::input_cap_bytes() == 256044u);
    CHECK(jalak::output_cap_bytes() == 720044u);
    CHECK(jalak::canonical_wav_bytes(0) == 44u);

    std::string reason;
    CHECK(!jalak::validate_capture_size(0, &reason));
    CHECK(reason == "empty capture");
    CHECK(!jalak::validate_capture_size(10, &reason));
    CHECK(!jalak::validate_capture_size(300000, &reason));
    CHECK(reason == "capture exceeds 8 s canonical cap");
    CHECK(jalak::validate_capture_size(256044, &reason));
    CHECK(reason == "ok");
}

}  // namespace

int main() {
    test_expression_clamp();
    test_framebuffer_page_bytes();
    test_draw_face_nonempty_and_bounded();
    test_state_machine_happy_path();
    test_state_machine_stop_and_reset();
    test_capture_cap_and_session_cap();
    test_turn_cap();
    test_book_handoff_is_quiet();
    test_gesture_classification();
    test_audio_limits();

    if (g_failures == 0) {
        std::printf("ALL HOST TESTS PASSED\n");
        return 0;
    }
    std::printf("%d HOST TEST(S) FAILED\n", g_failures);
    return 1;
}
