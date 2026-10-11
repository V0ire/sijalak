#pragma once

#include <cstdint>
#include <string>
#include "jalak/framebuffer.h"
#include "jalak/local/clip.h"
#include "jalak/local/sounds.h"

namespace jalak::local {
enum class Gesture { None, Tap, DoubleTap, Hold };
enum class Screen { Face, Menu, Timer };
enum class Phase { Focus, Break, Done };

class Button {
public:
    static constexpr std::uint64_t debounce_ms = 30, double_ms = 300, hold_ms = 1000;
    Gesture sample(bool pressed, std::uint64_t now);
private:
    bool initialized_ = false, raw_ = false, down_ = false, held_ = false;
    bool pending_ = false, second_ = false;
    std::uint64_t last_ = 0, edge_ = 0, press_ = 0, release_ = 0;
};

class Demo {
public:
    static constexpr std::uint64_t focus_ms = 25 * 60 * 1000, break_ms = 5 * 60 * 1000;
    void update(std::uint64_t now, Gesture gesture = Gesture::None);
    void render(Framebuffer& fb, bool reduced_motion = false) const;
    Screen screen() const { return screen_; }
    Phase phase() const { return phase_; }
    bool running() const { return running_; }
    unsigned selection() const { return selection_; }
    std::uint64_t remaining_ms() const { return remaining_; }

    // Optional Mochi animation for the Face screen. When a clip is loaded, its
    // current frame is drawn instead of the built-in eyes. Kept optional so the
    // demo still works with no SD card.
    bool load_face_clip(Clip&& clip) { return face_clip_.load(clip); }
    bool face_clip_loaded() const { return face_clip_.loaded(); }
    std::size_t face_frame_index() const { return face_clip_.frame_index(); }

    // Queue a sound to play for the last gesture/phase change. The caller drains
    // this and feeds it to the audio device; portable code never touches I2S.
    // `pop` means "play the Mochi pop from SD"; tones are synthesized.
    struct SoundRequest { bool play = false; bool is_pop = false; Tone tone = Tone::UiTap; };
    SoundRequest take_sound() { SoundRequest s = sound_; sound_ = {}; return s; }

private:
    Screen screen_ = Screen::Face;
    Phase phase_ = Phase::Focus;
    bool running_ = false, initialized_ = false, reacting_ = false;
    unsigned selection_ = 0;
    std::uint64_t last_ = 0, reaction_ = 0, remaining_ = focus_ms;
    ClipPlayer face_clip_;
    SoundRequest sound_{};

    void request_tone(Tone t) { sound_.play = true; sound_.is_pop = false; sound_.tone = t; }
    void request_pop() { sound_.play = true; sound_.is_pop = true; }
};
} // namespace jalak::local
