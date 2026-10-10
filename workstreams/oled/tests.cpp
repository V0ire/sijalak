#include "jalak/oled/adapter.h"
#include "jalak/hal/mock_display.h"
#include "jalak/oled_codec.h"

#include <array>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>

// Count host allocations around hot paths, including existing framebuffer code.
static std::size_t allocations = 0;
void* operator new(std::size_t size) {
    ++allocations;
    if (void* p = std::malloc(size == 0 ? 1 : size)) return p;
    throw std::bad_alloc();
}
void* operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }

using namespace jalak;
using namespace jalak::oled;
namespace {
int assertions = 0, passed = 0, failed = 0;
#define CHECK(condition) do { ++assertions; if (!(condition)) \
    throw std::runtime_error(std::string(#condition) + " @" + std::to_string(__LINE__)); } while (false)

constexpr std::array<State, 9> states{State::Boot, State::Ready, State::Listening,
    State::Uploading, State::Processing, State::Speaking, State::Stopped,
    State::Error, State::BookHandoff};

Snapshot input(State s) {
    Mood mood = Mood::Neutral;
    switch (s) {
        case State::Boot: case State::Stopped: mood = Mood::Sleepy; break;
        case State::Listening: mood = Mood::Listening; break;
        case State::Uploading: case State::Processing: mood = Mood::Thinking; break;
        case State::Speaking: mood = Mood::Speaking; break;
        case State::Error: mood = Mood::Error; break;
        case State::BookHandoff: mood = Mood::Happy; break;
        case State::Ready: break;
    }
    return {s, expression_for(mood), {s == State::Listening, s == State::Speaking, 60}};
}

std::vector<std::uint8_t> frame(Snapshot s, std::uint64_t time = 0,
                                Profile p = Profile::AdultLab, Motion m = Motion::Subtle) {
    Framebuffer fb;
    const auto info = render(fb, s, p, time, m);
    CHECK(info.geometry_ok);
    return fb.to_page_bytes();
}

int area_pixels(const Framebuffer& fb, int x0, int y0, int x1, int y1) {
    int count = 0;
    for (int y = y0; y < y1; ++y)
        for (int x = x0; x < x1; ++x) count += fb.get(x, y) ? 1 : 0;
    return count;
}

template <typename F> void test(const char* name, F action) {
    try { action(); ++passed; std::printf("PASS %s\n", name); }
    catch (const std::exception& ex) { ++failed; std::printf("FAIL %s: %s\n", name, ex.what()); }
}

class ProbeDisplay final : public hal::Display {
public:
    bool ready = true, success = true;
    unsigned pushes = 0;
    std::array<std::uint8_t, kPageBytes> last{};
    bool init(const hal::OledConfig&) override { ready = true; return true; }
    bool is_ready() const override { return ready; }
    bool present(const std::vector<std::uint8_t>& bytes) override {
        ++pushes;
        if (!success) return false;
        if (bytes.size() != last.size()) return false;
        std::copy(bytes.begin(), bytes.end(), last.begin());
        return true;
    }
};
}

int main() {
    test("all nine states render bounded monochrome frames", [] {
        Framebuffer fb;
        for (const auto state : states) {
            const auto info = render(fb, input(state), Profile::AdultLab, 700);
            CHECK(info.visible_state == state);
            CHECK(info.fault == ViewFault::None);
            CHECK(fb.to_page_bytes().size() == kPageBytes);
            const auto pixels = area_pixels(fb, 0, 0, 128, 64);
            CHECK(pixels > 100 && pixels < 2400);
            CHECK(area_pixels(fb, 0, 63, 128, 64) == 0);
        }
    });
    test("all states remain distinct without caption or animation", [] {
        std::array<std::vector<std::uint8_t>, 9> images;
        for (unsigned i = 0; i < states.size(); ++i) {
            Framebuffer fb;
            render(fb, input(states[i]), Profile::AdultLab, 900, Motion::Reduced);
            fb.fill_rect(0, 54, 128, 10, false);
            images[i] = fb.to_page_bytes();
        }
        for (unsigned i = 0; i < images.size(); ++i)
            for (unsigned j = i + 1; j < images.size(); ++j) CHECK(images[i] != images[j]);
    });
    test("ready idle blink closes briefly and returns exactly", [] {
        CHECK(frame(input(State::Ready), 0) == frame(input(State::Ready), 4100));
        CHECK(frame(input(State::Ready), 0) != frame(input(State::Ready), 4300));
        CHECK(frame(input(State::Ready), 0) == frame(input(State::Ready), 4440));
        CHECK(frame(input(State::Ready), 4300) == frame(input(State::Ready), 9100));
    });
    test("reduced motion preserves state and removes idle/pipeline motion", [] {
        for (const auto state : states)
            CHECK(frame(input(state), 0, Profile::AdultLab, Motion::Reduced) ==
                  frame(input(state), 4300, Profile::AdultLab, Motion::Reduced));
    });
    test("boot opens once then holds without auto-ready", [] {
        CHECK(frame(input(State::Boot), 0) != frame(input(State::Boot), 600));
        CHECK(frame(input(State::Boot), 600) == frame(input(State::Boot), UINT64_MAX));
        CHECK(inspect(input(State::Boot), Profile::AdultLab).visible_state == State::Boot);
    });
    test("stop deterministic across time stale flags and expression", [] {
        const auto expected = frame(input(State::Stopped));
        auto s = input(State::Stopped);
        s.activity = {true, true, 255};
        s.expression = expression_for(Mood::Mischievous);
        CHECK(frame(s, UINT64_MAX) == expected);
        CHECK(frame(s, 4300) == expected);
    });
    test("handoff is immediately still and does not solicit", [] {
        const auto expected = frame(input(State::BookHandoff));
        for (auto time : {1ull, 1000ull, 4800ull, 999999999999ull})
            CHECK(frame(input(State::BookHandoff), time) == expected);
        auto s = input(State::BookHandoff);
        s.expression = expression_for(Mood::Surprised);
        CHECK(frame(s) == expected);
    });
    test("child-local denies all adult pipeline states and capture flags", [] {
        for (const auto state : {State::Listening, State::Uploading, State::Processing}) {
            const auto info = inspect(input(state), Profile::ChildLocal);
            CHECK(info.visible_state == State::Error);
            CHECK(info.fault == ViewFault::ProfileMismatch);
            CHECK(std::string(caption(info, Profile::ChildLocal)) == "GALAT LOKAL");
        }
        auto s = input(State::Ready); s.activity.capture_active = true;
        CHECK(inspect(s, Profile::ChildLocal).fault == ViewFault::ProfileMismatch);
    });
    test("child-local disclosure remains visible in allowed states", [] {
        for (const auto state : {State::Boot, State::Ready, State::Speaking,
                                State::Stopped, State::Error, State::BookHandoff}) {
            const auto s = input(state);
            CHECK(inspect(s, Profile::ChildLocal).visible_state == state);
            CHECK(frame(s, 0, Profile::ChildLocal) != frame(s, 0, Profile::AdultLab));
        }
    });
    test("microphone cue requires actual exclusive capture", [] {
        auto s = input(State::Listening); s.activity.capture_active = false;
        CHECK(inspect(s, Profile::AdultLab).fault == ViewFault::CaptureMismatch);
        s.activity = {true, true, 10};
        CHECK(inspect(s, Profile::AdultLab).fault == ViewFault::PlaybackMismatch);
        for (const auto state : {State::Ready, State::Uploading, State::Processing,
                                State::Speaking, State::BookHandoff}) {
            s = input(state); s.activity.capture_active = true;
            CHECK(inspect(s, Profile::AdultLab).visible_state == State::Error);
        }
    });
    test("capture icon persists for every animation phase", [] {
        Framebuffer reference, actual;
        render(reference, input(State::Listening), Profile::AdultLab, 0);
        for (unsigned time = 0; time < 5000; time += 25) {
            render(actual, input(State::Listening), Profile::AdultLab, time);
            for (int y = 17; y < 41; ++y)
                for (int x = 3; x < 20; ++x) CHECK(actual.get(x,y) == reference.get(x,y));
        }
    });
    test("speech envelope only affects actual playback and clamps", [] {
        auto s = input(State::Speaking); s.activity.playback_active = false;
        const auto silence = frame(s);
        s.activity.speech_level_pct = 255;
        CHECK(frame(s) == silence);
        s.activity.playback_active = true; s.activity.speech_level_pct = 0;
        CHECK(frame(s) == silence);
        s.activity.speech_level_pct = 100;
        const auto loud = frame(s); CHECK(loud != silence);
        s.activity.speech_level_pct = 255; CHECK(frame(s) == loud);
    });
    test("invalid state and profile fail closed", [] {
        auto s = input(State::Ready); s.state = static_cast<State>(999);
        CHECK(inspect(s, Profile::AdultLab).fault == ViewFault::InvalidState);
        CHECK(inspect(input(State::Listening), static_cast<Profile>(999)).fault == ViewFault::ProfileMismatch);
    });
    test("wild expression values bounded and wink supported", [] {
        auto s = input(State::Ready);
        s.expression = {Mood::Curious, INT_MAX, INT_MIN, INT_MAX, INT_MIN, INT_MAX, true, false};
        Framebuffer fb;
        CHECK(render(fb, s, Profile::AdultLab, UINT64_MAX).geometry_ok);
        CHECK(area_pixels(fb, 20, 15, 105, 42) < 600);
        const auto wink = frame(s);
        s.expression.left_wink = false;
        CHECK(frame(s) != wink);
    });
    test("wrong framebuffer geometry is rejected untouched", [] {
        Framebuffer fb({64,32}); fb.fill_rect(0,0,4,4);
        const auto before = fb.to_page_bytes();
        CHECK(!render(fb, input(State::Ready), Profile::AdultLab, 0).geometry_ok);
        CHECK(fb.to_page_bytes() == before);
    });
    test("pure large timestamps reduce before narrowing", [] {
        for (const auto time : {UINT64_MAX, UINT64_MAX-1, UINT64_C(4294967295), UINT64_C(9007199254740993)}) {
            CHECK(frame(input(State::Ready), time) == frame(input(State::Ready), time % 4800u));
            CHECK(frame(input(State::Processing), time) == frame(input(State::Processing), time % 960u));
        }
    });
    test("default adapter is child-local and rate bounded", [] {
        Adapter a;
        CHECK(a.interval_ms() == 84);
        a.update(input(State::Listening), 0);
        CHECK(a.info().fault == ViewFault::ProfileMismatch);
        CHECK(Adapter({Profile::AdultLab, 0}).interval_ms() == 100);
        CHECK(Adapter({Profile::AdultLab, UINT_MAX}).interval_ms() == 67);
    });
    test("animation cadence drops missed frames without catch-up", [] {
        Adapter a({Profile::AdultLab});
        CHECK(a.update(input(State::Ready), 0) == Update::Rendered);
        CHECK(a.update(input(State::Ready), 83) == Update::RateLimited);
        CHECK(a.update(input(State::Ready), 84) == Update::Rendered);
        CHECK(a.update(input(State::Ready), UINT64_C(1000000000000)) == Update::Rendered);
        CHECK(a.update(input(State::Ready), UINT64_C(1000000000001)) == Update::RateLimited);
    });
    test("state and capture edges bypass cadence and busy audio", [] {
        Adapter a({Profile::AdultLab});
        a.update(input(State::Ready), 0);
        CHECK(a.update(input(State::Ready), 4300, true) == Update::AudioDeferred);
        CHECK(a.update(input(State::Listening), 1, true) == Update::Rendered);
        CHECK(a.info().visible_state == State::Listening);
        auto s = input(State::Listening); s.activity.capture_active = false;
        CHECK(a.update(s, 1, true) == Update::Rendered);
        CHECK(a.info().visible_state == State::Error);
    });
    test("stop replaces pending frame immediately from every state", [] {
        for (const auto state : states) {
            Adapter a({Profile::AdultLab}); ProbeDisplay display;
            a.update(input(state), 900);
            CHECK(a.update(input(State::Stopped), 900, true) ==
                  (state == State::Stopped ? Update::AudioDeferred : Update::Rendered));
            CHECK(a.page_bytes() == frame(input(State::Stopped)));
            CHECK(a.present(display, true) == Present::Sent);
            CHECK(std::equal(display.last.begin(), display.last.end(), a.page_bytes().begin()));
        }
    });
    test("display failure retains latest frame and can recover", [] {
        Adapter a({Profile::AdultLab}); ProbeDisplay display;
        CHECK(a.present(display) == Present::Clean);
        a.update(input(State::Listening), 0); display.ready = false;
        CHECK(a.present(display) == Present::NotReady); CHECK(a.dirty());
        display.ready = true; display.success = false;
        CHECK(a.present(display) == Present::Failed); CHECK(a.dirty());
        a.update(input(State::Stopped), 1); display.success = true;
        CHECK(a.present(display) == Present::Sent); CHECK(!a.dirty());
        CHECK(a.present(display) == Present::Clean);
        a.invalidate_display(); CHECK(a.present(display) == Present::Sent);
    });
    test("ordinary dirty animation yields but stop present bypasses", [] {
        Adapter a({Profile::AdultLab}); ProbeDisplay display;
        a.update(input(State::Ready), 0); a.present(display);
        a.update(input(State::Ready), 4300);
        CHECK(a.present(display, true) == Present::AudioDeferred);
        a.update(input(State::Stopped), 4300, true);
        CHECK(a.present(display, true) == Present::Sent);
    });
    test("unchanged and quiet frames avoid bus writes", [] {
        Adapter a({Profile::AdultLab}); ProbeDisplay display;
        a.update(input(State::BookHandoff), 0); a.present(display);
        for (unsigned t = 100; t <= 10000; t += 100) {
            a.update(input(State::BookHandoff), t);
            CHECK(a.present(display) == Present::Clean);
        }
        CHECK(display.pushes == 1);
    });
    test("clock64 rollover preserves cadence and animation", [] {
        Adapter wrapped({Profile::AdultLab}), ordinary({Profile::AdultLab});
        const auto start = UINT64_MAX - 40u;
        wrapped.update(input(State::Ready), start); ordinary.update(input(State::Ready), 0);
        CHECK(wrapped.update(input(State::Ready), 42) == Update::RateLimited);
        CHECK(wrapped.update(input(State::Ready), 43) == Update::Rendered);
        wrapped.update(input(State::Ready), start + 4300u);
        ordinary.update(input(State::Ready), 4300);
        CHECK(wrapped.page_bytes() == ordinary.page_bytes());
    });
    test("clock32 extension crosses wrap and drives same frame", [] {
        Millis32 clock;
        const auto first = clock.extend(UINT32_MAX - 20u);
        CHECK(clock.extend(20) - first == 41);
        CHECK(clock.extend(80) - first == 101);
        CHECK(clock.extend(80) - first == 101);
    });
    test("existing state machine bridge preserves expressions and transitions", [] {
        StateMachine machine;
        auto s = snapshot_from(machine);
        CHECK(s.state == State::Boot); CHECK(s.expression.mood == Mood::Sleepy);
        machine.handle(Event::BootComplete); machine.handle(Event::StartCapture);
        Adapter a({Profile::AdultLab});
        a.update(snapshot_from(machine, {true,false,0}), 0);
        CHECK(a.info().visible_state == State::Listening);
        CHECK(machine.state() == State::Listening); CHECK(machine.capture_elapsed_ms() == 0);
        machine.handle(Event::StopCapture);
        a.update(snapshot_from(machine), 1); CHECK(a.info().visible_state == State::Uploading);
        machine.handle(Event::UploadComplete);
        a.update(snapshot_from(machine), 2); CHECK(a.info().visible_state == State::Processing);
        machine.handle(Event::TurnSucceeded);
        a.update(snapshot_from(machine, {false,true,40}), 3);
        CHECK(a.info().visible_state == State::Speaking);
        machine.handle(Event::Stop);
        a.update(snapshot_from(machine), 3, true);
        CHECK(a.info().visible_state == State::Stopped);
    });
    test("packed pages match existing framebuffer mock and both codecs", [] {
        Adapter a({Profile::AdultLab}); hal::MockDisplay mock;
        mock.init({});
        for (const auto state : states) {
            a.update(input(state), 100);
            CHECK(a.page_bytes() == a.framebuffer().to_page_bytes());
            CHECK(a.present(mock) == Present::Sent);
            CHECK(mock.last_frame() == a.page_bytes());
            for (const auto kind : {OledControllerKind::Ssd1306, OledControllerKind::Sh1106}) {
                OledPanel panel; panel.controller = kind;
                const auto tx = oled_full_frame(panel, a.page_bytes());
                CHECK(tx.size() == 32);
                for (unsigned page = 0; page < 8; ++page)
                    CHECK(std::equal(tx[page*4+3].payload.begin(), tx[page*4+3].payload.end(),
                                     a.page_bytes().begin() + page*128));
            }
        }
    });
    test("render update and nonallocating display have no hot-path allocations", [] {
        Adapter a({Profile::AdultLab}); ProbeDisplay display;
        const auto before = allocations;
        for (unsigned i = 0; i < 1000; ++i) {
            a.update(input(states[i % states.size()]), static_cast<std::uint64_t>(i) * 100);
            a.present(display);
        }
        const auto after = allocations;
        CHECK(after == before);
    });
    std::printf("RESULT: %d passed, %d failed, %d assertions\n", passed, failed, assertions);
    return failed == 0 ? 0 : 1;
}
