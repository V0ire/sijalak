#include "jalak/local/demo.h"
#include <cstdlib>
#include <iostream>
#include <limits>
using namespace jalak::local;
unsigned checks = 0;
#define CHECK(x) do { ++checks; if (!(x)) { std::cerr << __LINE__ << ": " #x "\n"; std::exit(1); } } while (0)

void open_timer(Demo& d, std::uint64_t t) {
    d.update(t, Gesture::Tap); d.update(t, Gesture::Tap);
    d.update(t, Gesture::DoubleTap);
    CHECK(d.screen() == Screen::Timer);
}
void timer_tests() {
    Demo d;
    d.update(0); CHECK(!d.running()); CHECK(d.remaining_ms() == Demo::focus_ms);
    open_timer(d, 0);
    d.update(0, Gesture::DoubleTap); CHECK(d.running());
    d.update(Demo::focus_ms - 1); CHECK(d.phase() == Phase::Focus); CHECK(d.remaining_ms() == 1);
    d.update(Demo::focus_ms); CHECK(d.phase() == Phase::Break); CHECK(d.remaining_ms() == Demo::break_ms);
    d.update(Demo::focus_ms + 1); CHECK(d.remaining_ms() == Demo::break_ms - 1);
    d.update(Demo::focus_ms + Demo::break_ms - 1); CHECK(d.remaining_ms() == 1);
    d.update(Demo::focus_ms + Demo::break_ms); CHECK(d.phase() == Phase::Done); CHECK(!d.running()); CHECK(d.remaining_ms() == 0);
    d.update(Demo::focus_ms + Demo::break_ms + 1, Gesture::DoubleTap);
    CHECK(d.phase() == Phase::Focus); CHECK(d.running()); CHECK(d.remaining_ms() == Demo::focus_ms);
    d.update(Demo::focus_ms + Demo::break_ms + 2, Gesture::Hold);
    CHECK(d.screen() == Screen::Face); CHECK(!d.running()); CHECK(d.remaining_ms() == Demo::focus_ms);

    Demo paused; open_timer(paused, 100);
    paused.update(100, Gesture::DoubleTap);
    paused.update(1100, Gesture::DoubleTap); CHECK(!paused.running());
    const auto left = paused.remaining_ms();
    paused.update(1000000); CHECK(paused.remaining_ms() == left);
    paused.update(1000000, Gesture::DoubleTap);
    paused.update(1000001); CHECK(paused.remaining_ms() == left - 1);
    paused.update(10, Gesture::Hold); CHECK(paused.screen() == Screen::Timer); CHECK(paused.running());
    paused.update(1000002); CHECK(paused.remaining_ms() == left - 2);

    Demo menu; open_timer(menu, 0); menu.update(0, Gesture::DoubleTap);
    menu.update(1000, Gesture::Tap); CHECK(menu.screen() == Screen::Menu);
    menu.update(2000, Gesture::Hold); CHECK(menu.screen() == Screen::Face); CHECK(menu.running());
    CHECK(menu.remaining_ms() == Demo::focus_ms - 2000);
    menu.update(2000, Gesture::Tap); menu.update(2000, Gesture::Tap);
    menu.update(2000, Gesture::DoubleTap); CHECK(menu.screen() == Screen::Timer);
    menu.update(2000, Gesture::Hold); CHECK(!menu.running());

    for (const auto delta : {Demo::focus_ms + 7, Demo::focus_ms + Demo::break_ms + 1,
                            std::numeric_limits<std::uint64_t>::max()}) {
        Demo large; open_timer(large, 0); large.update(0, Gesture::DoubleTap); large.update(delta);
        if (delta == Demo::focus_ms + 7) CHECK(large.remaining_ms() == Demo::break_ms - 7);
        else { CHECK(large.phase() == Phase::Done); CHECK(!large.running()); }
    }
    Demo late; const auto base = (std::uint64_t{1} << 40);
    open_timer(late, base); late.update(base, Gesture::DoubleTap); late.update(base + 1);
    CHECK(late.remaining_ms() == Demo::focus_ms - 1);
}
void button_tests() {
    Button b;
    CHECK(b.sample(false, 0) == Gesture::None);
    CHECK(b.sample(true, 1) == Gesture::None);
    CHECK(b.sample(false, 10) == Gesture::None); // bounce
    CHECK(b.sample(false, 1000) == Gesture::None);
    b.sample(true, 1001); CHECK(b.sample(true, 1030) == Gesture::None);
    b.sample(true, 1031); b.sample(false, 1080); b.sample(false, 1110);
    CHECK(b.sample(false, 1410) == Gesture::None);
    CHECK(b.sample(false, 1411) == Gesture::Tap);
    CHECK(b.sample(false, 2000) == Gesture::None);
    for (const auto gap : {299u, 300u, 301u}) {
        Button dbl;
        dbl.sample(true, 0); dbl.sample(true, 30); dbl.sample(false, 60); dbl.sample(false, 90);
        const auto first = dbl.sample(true, 90 + gap);
        dbl.sample(true, 120 + gap); dbl.sample(false, 150 + gap);
        const auto result = dbl.sample(false, 180 + gap);
        if (gap <= 300) { CHECK(first == Gesture::None); CHECK(result == Gesture::DoubleTap); }
        else { CHECK(first == Gesture::Tap); CHECK(result == Gesture::None); CHECK(dbl.sample(false, 481 + gap) == Gesture::Tap); }
    }
    Button hold; hold.sample(true, 0); hold.sample(true, 30);
    CHECK(hold.sample(true, 1029) == Gesture::None);
    CHECK(hold.sample(true, 1030) == Gesture::Hold);
    CHECK(hold.sample(true, (std::uint64_t{1} << 40)) == Gesture::None);
    hold.sample(false, (std::uint64_t{1} << 40) + 1);
    CHECK(hold.sample(false, (std::uint64_t{1} << 40) + 31) == Gesture::None);
    CHECK(hold.sample(false, (std::uint64_t{1} << 40) + 1000) == Gesture::None);
    Button backwards; backwards.sample(false, 100); backwards.sample(true, 101);
    CHECK(backwards.sample(false, 1) == Gesture::None);
    backwards.sample(true, 131); CHECK(backwards.sample(true, 1131) == Gesture::Hold);
    Button second_hold; second_hold.sample(true, 0); second_hold.sample(true, 30);
    second_hold.sample(false, 60); second_hold.sample(false, 90);
    second_hold.sample(true, 120); second_hold.sample(true, 150);
    CHECK(second_hold.sample(true, 1150) == Gesture::Hold);
    second_hold.sample(false, 1200); second_hold.sample(false, 1230);
    CHECK(second_hold.sample(false, 2000) == Gesture::None);
    Button sparse; sparse.sample(true, 0); sparse.sample(true, 30);
    CHECK(sparse.sample(false, 1200) == Gesture::Hold);
    CHECK(sparse.sample(false, 1230) == Gesture::None);
    CHECK(sparse.sample(false, 2000) == Gesture::None);
    Button release_edge; release_edge.sample(true, 0); release_edge.sample(true, 30);
    CHECK(release_edge.sample(false, 1020) == Gesture::None);
    CHECK(release_edge.sample(false, 1050) == Gesture::Hold);
    CHECK(release_edge.sample(false, 2000) == Gesture::None);
}
void frame_tests() {
    Demo d; jalak::Framebuffer f({128,64});
    d.update(0); d.render(f); const auto open = f.to_page_bytes(); CHECK(open.size() == 1024);
    d.update(4400); d.render(f); CHECK(open != f.to_page_bytes());
    d.render(f, true); const auto still = f.to_page_bytes();
    d.update(8000); d.render(f, true); CHECK(still == f.to_page_bytes());
    d.update(8000, Gesture::DoubleTap); d.render(f); const auto wink = f.to_page_bytes();
    d.update(8700); d.render(f); CHECK(wink != f.to_page_bytes());
    open_timer(d, 9000); d.render(f); CHECK(f.to_page_bytes() != open);
    d.update(9000, Gesture::Tap); d.render(f); CHECK(f.get(8,22));
    d.update(9000, Gesture::Tap); d.render(f); CHECK(f.get(8,42)); CHECK(!f.get(8,22));
}
void anim_sound_tests() {
    using namespace jalak::local;
    // Load a 2-frame clip on the Face screen; render must draw the clip frame.
    Clip clip; clip.name = "t"; clip.frame_delay_ms = 100;
    clip.frames = {std::vector<std::uint8_t>(kFrameBytes, 0),
                   std::vector<std::uint8_t>(kFrameBytes, 0)};
    clip.frames[0][0] = 0x01;  // pixel (0,0)
    Demo d;
    CHECK(d.load_face_clip(std::move(clip)));
    CHECK(d.face_clip_loaded());
    jalak::Framebuffer f({128, 64});
    d.update(0);
    d.render(f);
    CHECK(f.get(0, 0));
    // The clip advances; the second frame is blank.
    d.update(100);
    d.render(f);
    CHECK(!f.get(0, 0));

    // Sound requests: tap emits a tone; a Face double-tap emits the Mochi pop.
    Demo s;
    s.update(0, Gesture::Tap);
    auto a = s.take_sound();
    CHECK(a.play && !a.is_pop);
    CHECK(!s.take_sound().play);  // drained

    Demo s2;  // fresh demo starts on the Face screen
    s2.update(0, Gesture::DoubleTap);
    auto b = s2.take_sound();
    CHECK(b.play && b.is_pop);
}

int main() { timer_tests(); button_tests(); frame_tests(); anim_sound_tests(); std::cout << checks << " local assertions passed\n"; }
