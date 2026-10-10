// Host tests for the HAL mock and a full render smoke test across moods.
// No I2C/I2S/GPIO. Confirms the mock display/audio contracts compile and that
// every mood renders a non-empty, bounded frame.

#include <cstdio>
#include <vector>

#include "jalak/framebuffer.h"
#include "jalak/hal/mock_audio.h"
#include "jalak/hal/mock_display.h"
#include "jalak/state_machine.h"

namespace {
int g_failures = 0;
#define CHECK(cond)                                                     \
    do {                                                                \
        if (!(cond)) {                                                  \
            std::printf("FAIL: %s (line %d)\n", #cond, __LINE__);       \
            ++g_failures;                                               \
        }                                                               \
    } while (0)

void test_mock_display_push() {
    jalak::hal::MockDisplay disp;
    jalak::hal::OledConfig cfg;
    cfg.controller = jalak::hal::OledController::Ssd1306;
    cfg.i2c_address = 0x3C;
    CHECK(disp.init(cfg));
    CHECK(disp.is_ready());

    jalak::Framebuffer fb({128, 64});
    jalak::draw_face(fb, jalak::expression_for(jalak::Mood::Happy));
    CHECK(disp.present(fb.to_page_bytes()));
    CHECK(disp.push_count() == 1);
    CHECK(disp.last_frame().size() == 128u * 8u);
}

void test_mock_audio_half_duplex_contract() {
    jalak::mock::MockAudioSink sink;
    CHECK(!sink.play({1, 2, 3}));  // cannot play before start
    CHECK(sink.start());
    CHECK(sink.play({1, 2, 3}));
    CHECK(sink.is_playing());
    sink.finish_playback();
    CHECK(!sink.is_playing());

    jalak::mock::MockAudioSource src;
    src.set_next_capture(std::vector<std::uint8_t>(100, 0xAB));
    CHECK(src.start_capture());
    CHECK(!src.start_capture());  // no overlapping capture
    auto got = src.stop_capture();
    CHECK(got.size() == 100);
    CHECK(!src.is_capturing());
}

void test_all_moods_render_nonempty_and_bounded() {
    const jalak::Mood moods[] = {
        jalak::Mood::Neutral, jalak::Mood::Happy,     jalak::Mood::Curious,
        jalak::Mood::Mischievous, jalak::Mood::Sleepy, jalak::Mood::Surprised,
        jalak::Mood::Annoyed, jalak::Mood::Thinking,  jalak::Mood::Listening,
        jalak::Mood::Speaking, jalak::Mood::Error,
    };
    for (auto m : moods) {
        jalak::Framebuffer fb({128, 64});
        const int painted = jalak::draw_face(fb, jalak::expression_for(m));
        CHECK(painted > 0);
        int count = 0;
        for (int y = 0; y < 64; ++y)
            for (int x = 0; x < 128; ++x)
                if (fb.get(x, y)) ++count;
        CHECK(count == painted);
        // Page bytes must be exactly the panel's page size.
        CHECK(fb.to_page_bytes().size() == 128u * 8u);
    }
}

void test_state_expressions_nonempty() {
    jalak::StateMachine sm;
    const jalak::Event path[] = {
        jalak::Event::BootComplete, jalak::Event::StartCapture,
        jalak::Event::StopCapture,  jalak::Event::UploadComplete,
        jalak::Event::TurnSucceeded, jalak::Event::SpeechFinished,
    };
    for (auto ev : path) {
        sm.handle(ev);
        jalak::Framebuffer fb({128, 64});
        CHECK(jalak::draw_face(fb, sm.current_expression()) > 0);
    }
    CHECK(sm.state() == jalak::State::Ready);
}

}  // namespace

int main() {
    test_mock_display_push();
    test_mock_audio_half_duplex_contract();
    test_all_moods_render_nonempty_and_bounded();
    test_state_expressions_nonempty();
    if (g_failures == 0) {
        std::printf("ALL HAL HOST TESTS PASSED\n");
        return 0;
    }
    std::printf("%d HAL HOST TEST(S) FAILED\n", g_failures);
    return 1;
}
