#pragma once
// Mock display used by host tests and by any build without real hardware.
// It records what would have been sent, so tests can assert rendering without
// claiming a real panel was driven.

#include <vector>

#include "jalak/hal/display.h"

namespace jalak::hal {

class MockDisplay : public Display {
public:
    bool init(const OledConfig& cfg) override {
        cfg_ = cfg;
        ready_ = true;
        return true;
    }
    bool present(const std::vector<std::uint8_t>& page_bytes) override {
        if (!ready_) return false;
        last_frame_ = page_bytes;
        ++push_count_;
        return true;
    }
    bool is_ready() const override { return ready_; }

    const OledConfig& config() const { return cfg_; }
    int push_count() const { return push_count_; }
    const std::vector<std::uint8_t>& last_frame() const { return last_frame_; }

private:
    OledConfig cfg_{};
    bool ready_ = false;
    int push_count_ = 0;
    std::vector<std::uint8_t> last_frame_;
};

}  // namespace jalak::hal
