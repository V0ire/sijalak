#include "jalak/oled/adapter.h"

#include <algorithm>

namespace jalak::oled {

Snapshot snapshot_from(StateMachine& machine, Activity activity) {
    return {machine.state(), machine.current_expression(), activity};
}

Adapter::Adapter(Config config)
    : config_(config), interval_ms_((1000u + std::clamp(config.fps, 10u, 15u) - 1u) /
                                    std::clamp(config.fps, 10u, 15u)),
      pages_(kPageBytes, 0) {}

Update Adapter::update(const Snapshot& snapshot, std::uint64_t now, bool audio_busy) {
    const auto next = inspect(snapshot, config_.profile);
    const bool state_changed = !initialized_ || next.visible_state != info_.visible_state ||
                               next.fault != info_.fault;
    const bool truth_changed = state_changed ||
        snapshot.activity.capture_active != previous_activity_.capture_active ||
        snapshot.activity.playback_active != previous_activity_.playback_active;
    if (!truth_changed) {
        if (audio_busy) return Update::AudioDeferred;
        if (now - last_render_ms_ < interval_ms_) return Update::RateLimited;
    }
    if (state_changed) entered_ms_ = now;
    info_ = render(framebuffer_, snapshot, config_.profile, now - entered_ms_, config_.motion);
    bool different = !initialized_;
    for (int page = 0; page < kHeight / 8; ++page) {
        for (int x = 0; x < kWidth; ++x) {
            std::uint8_t value = 0;
            for (unsigned bit = 0; bit < 8; ++bit)
                if (framebuffer_.get(x, page * 8 + static_cast<int>(bit)))
                    value |= static_cast<std::uint8_t>(1u << bit);
            const auto index = static_cast<unsigned>(page * kWidth + x);
            different = different || pages_[index] != value;
            pages_[index] = value;
        }
    }
    dirty_ = dirty_ || different;
    urgent_ = dirty_ && (urgent_ || truth_changed);
    previous_activity_ = snapshot.activity;
    last_render_ms_ = now;
    initialized_ = true;
    return Update::Rendered;
}

Present Adapter::present(hal::Display& display, bool audio_busy) {
    if (!initialized_ || !dirty_) return Present::Clean;
    if (audio_busy && !urgent_) return Present::AudioDeferred;
    if (!display.is_ready()) return Present::NotReady;
    if (!display.present(pages_)) return Present::Failed;
    dirty_ = urgent_ = false;
    return Present::Sent;
}

}  // namespace jalak::oled
