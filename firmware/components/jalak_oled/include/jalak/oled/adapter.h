#pragma once

#include <cstdint>
#include <vector>

#include "jalak/hal/display.h"
#include "jalak/oled/character.h"

namespace jalak::oled {

struct Config {
    Profile profile = Profile::ChildLocal;
    unsigned fps = 12;  // Clamped to 10..15, ceil interval never exceeds the cap.
    Motion motion = Motion::Subtle;
};

enum class Update { Rendered, RateLimited, AudioDeferred };
enum class Present { Sent, Clean, AudioDeferred, NotReady, Failed };

// Read-only bridge to the current core API. Activity must come from the actual
// audio owner. The expression and state are copied; no Event is delivered.
Snapshot snapshot_from(StateMachine& machine, Activity activity = {});

// Single-owner UI-task object. External synchronization is required if the
// controller and display live on different tasks; never call it from an ISR.
class Adapter {
public:
    explicit Adapter(Config config = {});

    // Use monotonic uint64 milliseconds (including natural unsigned rollover).
    // Changed state/activity truth bypasses cadence and audio deferral. Unchanged
    // animation yields to audio and never catches up missed frames.
    Update update(const Snapshot& snapshot, std::uint64_t now_ms,
                  bool audio_busy = false);
    Present present(hal::Display& display, bool audio_busy = false);

    // Call after reconnect/re-init: the panel may have lost its contents.
    void invalidate_display() { dirty_ = true; urgent_ = true; }

    const Framebuffer& framebuffer() const { return framebuffer_; }
    const std::vector<std::uint8_t>& page_bytes() const { return pages_; }
    const RenderInfo& info() const { return info_; }
    unsigned interval_ms() const { return interval_ms_; }
    bool dirty() const { return dirty_; }

private:
    const Config config_;
    const unsigned interval_ms_;
    Framebuffer framebuffer_{{kWidth, kHeight}};
    std::vector<std::uint8_t> pages_;
    RenderInfo info_{};
    Activity previous_activity_{};
    std::uint64_t entered_ms_ = 0;
    std::uint64_t last_render_ms_ = 0;
    bool initialized_ = false;
    bool dirty_ = false;
    bool urgent_ = false;
};

// Optional bridge for a wrapping 32-bit millisecond source. Sample at least
// once per 2^32 ms; multiple missed wraps cannot be reconstructed. No signed math.
class Millis32 {
public:
    std::uint64_t extend(std::uint32_t now) {
        if (!initialized_) { previous_ = now; total_ = now; initialized_ = true; }
        else { total_ += static_cast<std::uint32_t>(now - previous_); previous_ = now; }
        return total_;
    }
private:
    std::uint64_t total_ = 0;
    std::uint32_t previous_ = 0;
    bool initialized_ = false;
};

}  // namespace jalak::oled
