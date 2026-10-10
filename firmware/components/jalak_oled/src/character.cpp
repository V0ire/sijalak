#include "jalak/oled/character.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>

namespace jalak::oled {
namespace {

// Private pixel vocabulary: two-pixel strokes, rounded tips, one-bit contrast.
// The unequal crest and folded cheek feathers belong to this character only.
void stroke(Framebuffer& fb, int x0, int y0, int x1, int y1, bool on = true) {
    const int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    const int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;
    for (;;) {
        fb.fill_rect(x0, y0, 2, 2, on);
        if (x0 == x1 && y0 == y1) break;
        const int twice = 2 * error;
        if (twice >= dy) { error += dy; x0 += sx; }
        if (twice <= dx) { error += dx; y0 += sy; }
    }
}

// Small secondary captions; state recognition must also survive without text.
// Each row is five bits. This authored bitmap alphabet is not a font dependency.
std::array<unsigned, 7> glyph(char c) {
    switch (c) {
        case 'A': return {14,17,17,31,17,17,17};
        case 'B': return {30,17,17,30,17,17,30};
        case 'E': return {31,16,16,30,16,16,31};
        case 'G': return {14,17,16,23,17,17,14};
        case 'H': return {17,17,17,31,17,17,17};
        case 'I': return {14,4,4,4,4,4,14};
        case 'K': return {17,18,20,24,20,18,17};
        case 'L': return {16,16,16,16,16,16,31};
        case 'M': return {17,27,21,21,17,17,17};
        case 'N': return {17,25,25,21,19,19,17};
        case 'O': return {14,17,17,17,17,17,14};
        case 'P': return {30,17,17,30,16,16,16};
        case 'R': return {30,17,17,30,20,18,17};
        case 'S': return {15,16,16,14,1,1,30};
        case 'T': return {31,4,4,4,4,4,4};
        case 'U': return {17,17,17,17,17,17,14};
        default: return {};
    }
}

void text(Framebuffer& fb, const char* value) {
    const int count = static_cast<int>(std::strlen(value));
    int x = (kWidth - (count * 6 - 1)) / 2;
    for (int i = 0; i < count; ++i, x += 6) {
        const auto rows = glyph(value[i]);
        for (int y = 0; y < 7; ++y)
            for (int bit = 0; bit < 5; ++bit)
                if ((rows[static_cast<unsigned>(y)] & (1u << (4 - bit))) != 0)
                    fb.set(x + bit, 55 + y);
    }
}

void book(Framebuffer& fb, int x, int y) {
    stroke(fb, x, y, x + 6, y + 2);
    stroke(fb, x + 6, y + 2, x + 12, y);
    stroke(fb, x, y, x, y + 10);
    stroke(fb, x + 12, y, x + 12, y + 10);
    stroke(fb, x, y + 10, x + 6, y + 12);
    stroke(fb, x + 6, y + 12, x + 12, y + 10);
    stroke(fb, x + 6, y + 2, x + 6, y + 12);
}

void microphone(Framebuffer& fb) {
    fb.fill_rounded_rect(8, 18, 7, 12, 3);
    stroke(fb, 5, 26, 5, 31);
    stroke(fb, 5, 31, 10, 34);
    stroke(fb, 10, 34, 17, 31);
    stroke(fb, 17, 31, 17, 26);
    stroke(fb, 11, 34, 11, 38);
    stroke(fb, 7, 38, 15, 38);
}

void eye(Framebuffer& fb, int cx, int cy, int aperture, bool wink,
         int brow, bool outward) {
    if (wink || aperture < 8) {
        stroke(fb, cx - 11, cy, cx + 10, cy);
        return;
    }
    const int height = std::max(3, 23 * aperture / 100);
    const int top = cy - height / 2;
    fb.fill_rounded_rect(cx - 12, top, 25, height, std::min(6, height / 2));
    // A small upper-outer cut makes the eye area a folded feather, not a pupil.
    const int notch = outward ? cx + 8 : cx - 11;
    fb.fill_rect(notch, top, 4, 3, false);
    if (brow != 0) {
        const int tilt = std::clamp(brow / 3, -2, 2);
        stroke(fb, cx - 8, top - 5 + (outward ? tilt : -tilt),
               cx + 7, top - 5 + (outward ? -tilt : tilt));
    }
}

void state_symbol(Framebuffer& fb, State state, std::uint64_t elapsed,
                  bool animate, bool playing, int level) {
    const int beat = animate ? static_cast<int>((elapsed / 320u) % 3u) : 1;
    switch (state) {
        case State::Boot:
            // Opening brackets, not a fake completion percentage.
            stroke(fb, 6, 23, 6, 34);
            stroke(fb, 6, 23, 13, 23);
            stroke(fb, 6, 34, 13, 34);
            stroke(fb, 113, 23, 120, 23);
            stroke(fb, 120, 23, 120, 34);
            stroke(fb, 113, 34, 120, 34);
            break;
        case State::Ready:
            break;
        case State::Listening:
            microphone(fb); // Persistent: blinking never erases capture status.
            fb.fill_rect(110, 22, 3, 17);
            fb.fill_rect(116, 26 - beat, 3, 9 + beat * 2);
            break;
        case State::Uploading:
            stroke(fb, 11, 35, 11, 20);
            stroke(fb, 5, 26, 11, 20);
            stroke(fb, 11, 20, 17, 26);
            stroke(fb, 4, 39, 18, 39);
            fb.fill_rect(111, 34 - beat * 5, 6, 3);
            break;
        case State::Processing:
            // Three hollow thought dots; no microphone/waveform/progress claim.
            for (int i = 0; i < 3; ++i) {
                fb.fill_rect(109, 20 + i * 8, 6, 5);
                if (i != beat) fb.fill_rect(111, 22 + i * 8, 2, 1, false);
            }
            break;
        case State::Speaking:
            fb.fill_rect(6, 26, 4, 8);
            stroke(fb, 10, 26, 16, 21);
            stroke(fb, 16, 21, 16, 38);
            stroke(fb, 16, 38, 10, 33);
            if (playing && level > 0) {
                const int height = 4 + level * 12 / 100;
                fb.fill_rect(110, 30 - height / 2, 3, height);
                fb.fill_rect(116, 27, 3, 6);
            }
            break;
        case State::Stopped:
            fb.fill_rounded_rect(6, 23, 13, 13, 2); // Solid stop square, no blink.
            break;
        case State::Error:
            stroke(fb, 5, 24, 10, 19);
            stroke(fb, 10, 19, 16, 24);
            stroke(fb, 16, 24, 16, 34);
            stroke(fb, 16, 34, 10, 39);
            stroke(fb, 10, 39, 5, 34);
            stroke(fb, 5, 34, 5, 24);
            fb.fill_rect(10, 24, 2, 7);
            fb.fill_rect(10, 34, 2, 2);
            break;
        case State::BookHandoff:
            book(fb, 108, 25);
            break;
    }
}

bool valid_state(State state) {
    switch (state) {
        case State::Boot: case State::Ready: case State::Listening:
        case State::Uploading: case State::Processing: case State::Speaking:
        case State::BookHandoff: case State::Stopped: case State::Error:
            return true;
    }
    return false;
}
}  // namespace

RenderInfo inspect(const Snapshot& s, Profile profile) {
    if (!valid_state(s.state)) return {State::Error, ViewFault::InvalidState, true};
    // STOP is visually deterministic even if the controller snapshot still
    // contains old RX/TX flags. It does NOT itself stop any audio hardware.
    if (s.state == State::Stopped) return {State::Stopped, ViewFault::None, true};
    const bool child = profile != Profile::AdultLab; // Unknown profile fails local.
    if (child && (s.state == State::Listening || s.state == State::Uploading ||
                  s.state == State::Processing || s.activity.capture_active))
        return {State::Error, ViewFault::ProfileMismatch, true};
    if (s.activity.capture_active != (s.state == State::Listening))
        return {State::Error, ViewFault::CaptureMismatch, true};
    if (s.activity.playback_active && s.state != State::Speaking)
        return {State::Error, ViewFault::PlaybackMismatch, true};
    return {s.state, ViewFault::None, true};
}

const char* caption(const RenderInfo& info, Profile profile) {
    if (info.fault == ViewFault::ProfileMismatch) return "GALAT LOKAL";
    if (info.fault != ViewFault::None) return "GALAT";
    switch (info.visible_state) {
        case State::Boot: return "MULAI";
        case State::Ready: return profile == Profile::AdultLab ? "SIAP" : "SIAP LOKAL";
        case State::Listening: return "REKAM";
        case State::Uploading: return "KIRIM";
        case State::Processing: return "PROSES";
        case State::Speaking: return profile == Profile::AdultLab ? "SUARA" : "SUARA LOKAL";
        case State::Stopped: return "STOP";
        case State::Error: return "GALAT";
        case State::BookHandoff: return "BUKU";
    }
    return "GALAT";
}

RenderInfo render(Framebuffer& fb, const Snapshot& s, Profile profile,
                  std::uint64_t elapsed, Motion motion) {
    auto info = inspect(s, profile);
    if (fb.width() != kWidth || fb.height() != kHeight) {
        info.geometry_ok = false;
        return info;
    }
    fb.clear();
    const State state = info.visible_state;
    const bool animate = motion == Motion::Subtle;
    Expression e = clamp_expression(s.expression, {kWidth, kHeight});
    e.look_dx = std::clamp(e.look_dx, -3, 3);
    e.look_dy = std::clamp(e.look_dy, -2, 2);
    int left = e.left_aperture_pct, right = e.right_aperture_pct;
    int dx = e.look_dx, dy = e.look_dy, brow = e.brow_tilt;
    bool left_wink = e.left_wink, right_wink = e.right_wink;
    bool calm = false;
    switch (state) {
        case State::Boot:
            left = right = (!animate || elapsed >= 600u) ? 75 :
                (elapsed < 240u ? 8 : (elapsed < 400u ? 35 : 60));
            dx = dy = brow = 0;
            left_wink = right_wink = false;
            break;
        case State::Ready:
            // Short full blink once per 4.8s; no bobbing, reward, or solicitation.
            if (animate) {
                const auto phase = elapsed % 4800u;
                if (phase >= 4200u && phase < 4440u) {
                    const int aperture = phase < 4280u ? 40 : (phase < 4360u ? 0 : 40);
                    left = std::min(left, aperture); right = std::min(right, aperture);
                }
            }
            break;
        case State::Listening:
            left = std::max(90, left); right = std::max(90, right);
            left_wink = right_wink = false;
            dx = 0; dy = -1; brow = 3;
            break;
        case State::Uploading:
            left = std::min(left, 75); right = std::min(right, 75);
            dy = -2; dx = 0; brow = 0;
            break;
        case State::Processing:
            left = std::min(left, 75); right = std::min(right, 48);
            dx = -2; dy = -1; brow = 3;
            break;
        case State::Speaking:
            left = std::min(left, 85); right = std::min(right, 85);
            break;
        case State::Stopped:
            left = right = 0; dx = dy = brow = 0;
            left_wink = right_wink = false;
            break;
        case State::Error:
            left = 50; right = 30; dx = 0; dy = 1; brow = 3;
            left_wink = right_wink = false;
            break;
        case State::BookHandoff:
            calm = true; dx = dy = brow = 0;
            break;
    }

    // Twin blunt crest nubs: deliberately unequal and below the active edge.
    fb.fill_rounded_rect(55, 4, 6, 7, 2);
    fb.fill_rounded_rect(65, 2, 7, 9, 3);
    const int cy = 29 + dy;
    if (calm) {
        for (const int cx : {44, 83}) {
            stroke(fb, cx - 10, 28, cx - 6, 31);
            stroke(fb, cx - 6, 31, cx + 5, 31);
            stroke(fb, cx + 5, 31, cx + 9, 28);
        }
    } else {
        eye(fb, 44 + dx, cy, left, left_wink, brow, false);
        eye(fb, 83 + dx, cy, right, right_wink, brow, true);
    }
    // Folded-wing cheeks frame one face region without a miniature body.
    stroke(fb, 25, 39, 29, 42); stroke(fb, 29, 42, 33, 42);
    stroke(fb, 94, 42, 98, 42); stroke(fb, 98, 42, 102, 39);
    // Tiny beak: only measured, active playback can open it. Zero is silence.
    const int level = std::min(100, static_cast<int>(s.activity.speech_level_pct));
    const bool talking = state == State::Speaking && s.activity.playback_active;
    const int open = talking && level > 0 && animate ? 2 + level * 5 / 100 : 0;
    stroke(fb, 59, 43, 64, 46 + open);
    stroke(fb, 64, 46 + open, 69, 43);
    if (open > 0) stroke(fb, 59, 43, 69, 43);
    state_symbol(fb, state, elapsed, animate, talking, level);
    text(fb, caption(info, profile));
    // Persistent tiny book marker discloses local content even outside captions.
    if (profile != Profile::AdultLab) book(fb, 109, 3);
    return info;
}

}  // namespace jalak::oled
