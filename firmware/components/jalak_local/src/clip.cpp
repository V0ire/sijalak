#include "jalak/local/clip.h"

#include <cstring>

namespace jalak::local {

std::vector<std::vector<std::uint8_t>> unpack_frames(const std::vector<std::uint8_t>& blob,
                                                     std::size_t frame_count) {
    std::vector<std::vector<std::uint8_t>> out;
    if (frame_count == 0) return out;
    // Fail-closed on any size mismatch: never read past the buffer.
    if (blob.size() != frame_count * kFrameBytes) return out;
    out.reserve(frame_count);
    for (std::size_t i = 0; i < frame_count; ++i) {
        out.emplace_back(blob.begin() + static_cast<std::ptrdiff_t>(i * kFrameBytes),
                         blob.begin() + static_cast<std::ptrdiff_t>((i + 1) * kFrameBytes));
    }
    return out;
}

bool ClipPlayer::load(const Clip& clip) {
    clear();
    if (clip.frames.empty()) return false;
    for (const auto& f : clip.frames) {
        if (f.size() != kFrameBytes) return false;  // reject a bad clip wholesale
    }
    frames_ = clip.frames;
    delay_ms_ = clip.frame_delay_ms > 0 ? clip.frame_delay_ms : 100;
    index_ = 0;
    started_ = false;
    return true;
}

void ClipPlayer::clear() {
    frames_.clear();
    index_ = 0;
    started_ = false;
}

bool ClipPlayer::tick(std::uint64_t now_ms) {
    if (frames_.empty()) return false;
    if (!started_) {
        started_ = true;
        last_ = now_ms;
        return true;
    }
    if (now_ms < last_) return false;  // ignore backwards time entirely
    const std::uint64_t delay = static_cast<std::uint64_t>(delay_ms_);
    if (now_ms - last_ < delay) return false;
    // Advance one or more frames; when a large delta spans many frames, jump
    // ahead without a per-frame catch-up loop (bounded work).
    std::uint64_t advanced = (now_ms - last_) / delay;
    last_ += advanced * delay;
    if (advanced > frames_.size()) advanced = frames_.size();  // no overflow/short-circuit
    index_ = static_cast<std::size_t>((index_ + advanced) % frames_.size());
    return true;
}

bool ClipPlayer::page_bytes(std::vector<std::uint8_t>& out) const {
    if (frames_.empty()) return false;
    out = frames_[index_];
    return true;
}

bool ClipPlayer::draw(Framebuffer& fb) const {
    if (frames_.empty()) return false;
    blit_page_bytes(frames_[index_], fb);
    return true;
}

void blit_page_bytes(const std::vector<std::uint8_t>& bytes, Framebuffer& fb) {
    const PanelGeometry geo = fb.geometry();
    const int pages = (geo.height + 7) / 8;
    const std::size_t need = static_cast<std::size_t>(geo.width) * static_cast<std::size_t>(pages);
    if (bytes.size() < need) return;  // fail-closed
    fb.clear();
    for (int page = 0; page < pages; ++page) {
        for (int x = 0; x < geo.width; ++x) {
            const std::uint8_t b = bytes[static_cast<std::size_t>(page) * geo.width + x];
            for (int bit = 0; bit < 8; ++bit) {
                const int y = page * 8 + bit;
                if (y >= geo.height) break;
                if (b & (1u << bit)) fb.set(x, y, true);
            }
        }
    }
}

}  // namespace jalak::local
