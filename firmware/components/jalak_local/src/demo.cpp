#include "jalak/local/demo.h"

#include <cstdio>

namespace jalak::local {
Gesture Button::sample(bool pressed, std::uint64_t now) {
    if (initialized_ && now < last_) return Gesture::None;
    if (!initialized_) { initialized_ = true; edge_ = now; }
    last_ = now;
    if (pressed != raw_) { raw_ = pressed; edge_ = now; }
    Gesture event = Gesture::None;
    if (raw_ != down_ && now - edge_ >= debounce_ms) {
        down_ = raw_;
        if (down_) {
            press_ = now;
            held_ = false;
            second_ = pending_ && edge_ - release_ <= double_ms;
        } else if (!held_) {
            if (now - press_ >= hold_ms) {
                held_ = true; pending_ = second_ = false;
                return Gesture::Hold;
            }
            if (second_) { event = Gesture::DoubleTap; pending_ = false; }
            else { pending_ = true; release_ = now; }
            second_ = false;
        }
    }
    if (down_ && !held_ && now - press_ >= hold_ms) {
        held_ = true;
        pending_ = second_ = false;
        return Gesture::Hold;
    }
    // A second raw edge reserves the tap window through debounce, but not forever.
    if (pending_ && !second_ && now - release_ > double_ms &&
        !(raw_ && !down_ && edge_ >= release_ && edge_ - release_ <= double_ms)) {
        pending_ = false;
        return Gesture::Tap;
    }
    return event;
}

void Demo::update(std::uint64_t now, Gesture gesture) {
    if (initialized_ && now < last_) return; // Ignore sample until clock catches up.
    std::uint64_t delta = initialized_ ? now - last_ : 0;
    initialized_ = true;
    last_ = now;
    if (face_clip_.loaded()) face_clip_.tick(now);
    if (running_) {
        if (delta >= remaining_) {
            delta -= remaining_;
            if (phase_ == Phase::Focus) {
                phase_ = Phase::Break;
                remaining_ = break_ms;
                request_tone(Tone::BreakStart);
            } else { phase_ = Phase::Done; running_ = false; remaining_ = 0; request_tone(Tone::SessionEnd); }
            if (running_ && delta >= remaining_) {
                phase_ = Phase::Done; running_ = false; remaining_ = 0;
            } else if (running_) remaining_ -= delta;
        } else remaining_ -= delta;
    }
    switch (gesture) {
        case Gesture::Tap:
            request_tone(Tone::UiTap);
            if (screen_ == Screen::Menu) selection_ ^= 1u;
            else { screen_ = Screen::Menu; selection_ = 0; }
            break;
        case Gesture::DoubleTap:
            request_tone(Tone::UiSelect);
            if (screen_ == Screen::Menu) {
                screen_ = selection_ == 0 ? Screen::Face : Screen::Timer;
                if (screen_ == Screen::Timer && remaining_ == focus_ms && !running_) {
                    request_tone(Tone::FocusStart);
                }
            } else if (screen_ == Screen::Timer) {
                if (phase_ == Phase::Done) {
                    phase_ = Phase::Focus; remaining_ = focus_ms;
                    request_tone(Tone::FocusStart);
                } else if (!running_ && remaining_ == focus_ms) {
                    request_tone(Tone::FocusStart);
                }
                running_ = !running_;
            } else {
                reacting_ = true; reaction_ = now;
                request_pop();  // face tap plays the Mochi pop, like upstream
            }
            break;
        case Gesture::Hold:
            if (screen_ == Screen::Timer) {
                phase_ = Phase::Focus; remaining_ = focus_ms; running_ = false;
            }
            screen_ = Screen::Face;
            break;
        case Gesture::None: break;
    }
}

namespace {
// Original 3x5 glyphs, row-major bits. Only the demo's short labels are needed.
std::uint16_t glyph(char c) {
    switch (c) {
        case '0': return 0b111101101101111; case '1': return 0b010110010010111;
        case '2': return 0b111001111100111; case '3': return 0b111001111001111;
        case '4': return 0b101101111001001; case '5': return 0b111100111001111;
        case '6': return 0b111100111101111; case '7': return 0b111001010010010;
        case '8': return 0b111101111101111; case '9': return 0b111101111001111;
        case ':': return 0b000010000010000;
        case 'A': return 0b010101111101101; case 'B': return 0b110101110101110;
        case 'D': return 0b110101101101110; case 'E': return 0b111100110100111;
        case 'F': return 0b111100110100100; case 'H': return 0b101101111101101;
        case 'I': return 0b111010010010111; case 'J': return 0b001001001101111;
        case 'K': return 0b101101110101101; case 'L': return 0b100100100100111;
        case 'M': return 0b101111111101101; case 'N': return 0b101111111111101;
        case 'O': return 0b010101101101010; case 'P': return 0b110101110100100;
        case 'R': return 0b110101110101101; case 'S': return 0b111100111001111;
        case 'T': return 0b111010010010010; case 'U': return 0b101101101101111;
        case 'W': return 0b101101111111101; default: return 0;
    }
}
void text(Framebuffer& fb, int x, int y, const char* s, int scale = 2) {
    for (; *s; ++s, x += 4 * scale) {
        const auto bits = glyph(*s);
        for (int row = 0; row < 5; ++row)
            for (int col = 0; col < 3; ++col)
                if (bits & (1u << (14 - row * 3 - col)))
                    fb.fill_rect(x + col * scale, y + row * scale, scale, scale);
    }
}
}

void Demo::render(Framebuffer& fb, bool reduced_motion) const {
    fb.clear();
    if (screen_ == Screen::Face) {
        if (face_clip_.loaded()) {
            // Mochi animation replaces the built-in eyes. If reduced motion is
            // requested, freeze on frame 0 rather than redraw per tick.
            face_clip_.draw(fb);
            return;
        }
        const auto beat = last_ % 4800;
        const bool blink = !reduced_motion && beat >= 4400 && beat < 4520;
        const bool wink = !reduced_motion && reacting_ && last_ - reaction_ < 700;
        const int dx = reduced_motion ? 0 : (last_ / 1600 % 3 == 0 ? -3 : last_ / 1600 % 3 == 2 ? 3 : 0);
        for (int eye = 0; eye < 2; ++eye) {
            const int x = 27 + eye * 48 + dx;
            if (blink || (wink && eye == 1)) fb.fill_rect(x, 29, 25, 3);
            else fb.fill_rounded_rect(x, 18, 25, 26, 7);
        }
        text(fb, 44, 54, "LOKAL", 1);
    } else if (screen_ == Screen::Menu) {
        text(fb, 8, 4, "MENU", 1);
        text(fb, 22, 20, "WAJAH");
        text(fb, 22, 40, "POMODORO");
        fb.fill_rect(8, selection_ == 0 ? 22 : 42, 6, 6);
    } else {
        text(fb, 8, 3, phase_ == Phase::Focus ? "FOKUS" : phase_ == Phase::Break ? "JEDA" : "SELESAI", 1);
        const auto seconds = remaining_ / 1000 + (remaining_ % 1000 != 0);
        char time[16];
        std::snprintf(time, sizeof time, "%02u:%02u", static_cast<unsigned>(seconds / 60), static_cast<unsigned>(seconds % 60));
        text(fb, 25, 20, time, 4);
        text(fb, 8, 51, phase_ == Phase::Done ? "ULANG" : running_ ? "JALAN" : remaining_ == focus_ms ? "MULAI" : "PAUSE", 1);
        text(fb, 85, 51, "LOKAL", 1);
    }
}
} // namespace jalak::local
