#pragma once
// Portable animation player for the OLED demo.
//
// The device reads PACKED 1-BIT FRAMES produced offline by tools/mochi_assets.py
// (128x64 => 1024 bytes/frame). Decoding GIF on-device would cost CPU/RAM the
// OLED + I2S paths can't spare, so the demo ships pre-packed frames instead.
// This keeps the runtime tiny and fully host-testable; the SD layer only has to
// hand back raw bytes.
//
// This module has no ESP-IDF dependency. The SD/FATFS read lives behind the
// FrameSource interface so tests can feed bytes from memory.

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "jalak/framebuffer.h"

namespace jalak::local {

constexpr int kFrameW = 128;
constexpr int kFrameH = 64;
constexpr std::size_t kFrameBytes = static_cast<std::size_t>(kFrameW) * (kFrameH / 8);  // 1024

// One animation = an ordered list of packed frames + per-frame delay.
struct Clip {
    std::string name;
    std::vector<std::vector<std::uint8_t>> frames;  // each kFrameBytes long
    int frame_delay_ms = 100;
};

// Parse the concatenated frame blob (+ frame count) from a .bin file.
// Returns empty on any size mismatch (fail-closed; never reads past the buffer).
std::vector<std::vector<std::uint8_t>> unpack_frames(const std::vector<std::uint8_t>& blob,
                                                     std::size_t frame_count);

// Playback state machine. Pure logic: `tick(now_ms)` advances frames; nothing
// here touches SD, I2S, or the panel.
class ClipPlayer {
public:
    // Load a clip. Frames shorter than kFrameBytes are rejected (fail-closed).
    bool load(const Clip& clip);
    void clear();

    bool loaded() const { return !frames_.empty(); }
    std::size_t frame_count() const { return frames_.size(); }
    std::size_t frame_index() const { return index_; }
    int frame_delay_ms() const { return delay_ms_; }

    // Advance using an absolute monotonic timestamp. Backwards time is ignored.
    // Returns true when the displayed frame changed.
    bool tick(std::uint64_t now_ms);

    // Copy the current packed frame into `out` (resized to kFrameBytes).
    // Returns false if nothing is loaded.
    bool page_bytes(std::vector<std::uint8_t>& out) const;

    // Draw the current frame straight into a framebuffer (for the preview/tests).
    bool draw(Framebuffer& fb) const;

private:
    std::vector<std::vector<std::uint8_t>> frames_;
    std::size_t index_ = 0;
    int delay_ms_ = 100;
    std::uint64_t last_ = 0;
    bool started_ = false;
};

// Unpack a packed 1-bit page buffer into a Framebuffer (LSB = topmost pixel).
void blit_page_bytes(const std::vector<std::uint8_t>& bytes, Framebuffer& fb);

}  // namespace jalak::local
