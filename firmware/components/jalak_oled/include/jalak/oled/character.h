#pragma once

#include <cstdint>

#include "jalak/framebuffer.h"
#include "jalak/state_machine.h"

namespace jalak::oled {

constexpr int kWidth = 128;
constexpr int kHeight = 64;
constexpr unsigned kPageBytes = 1024;

// Presentation policy, NOT build/deployment authorization. Fixed by the owner
// constructing Adapter, never selected by a visitor control.
enum class Profile { ChildLocal, AdultLab };
enum class Motion { Subtle, Reduced };
enum class ViewFault { None, InvalidState, ProfileMismatch, CaptureMismatch,
                       PlaybackMismatch };

struct Activity {
    bool capture_active = false;   // Actual RX state, not PTT intent.
    bool playback_active = false;  // Actual TX state, not reply availability.
    std::uint8_t speech_level_pct = 0;  // Optional measured TX envelope, 0..100.
};

struct Snapshot {
    State state = State::Boot;
    Expression expression{};       // Existing expression API remains authoritative.
    Activity activity{};
};

struct RenderInfo {
    State visible_state = State::Boot;
    ViewFault fault = ViewFault::None;
    bool geometry_ok = true;
};

// Pure presentation guard: does not transition, tick, or mutate the controller.
// STOP overrides stale activity. Unsupported child pipeline states show ERROR.
RenderInfo inspect(const Snapshot& snapshot, Profile profile);
const char* caption(const RenderInfo& info, Profile profile);

// Draws the original character into the existing framebuffer. Only 128x64 is
// supported; other geometries are returned untouched with geometry_ok=false.
// No allocation, I/O, sleep, random source, float clock, or catch-up loop.
RenderInfo render(Framebuffer& fb, const Snapshot& snapshot, Profile profile,
                  std::uint64_t elapsed_ms, Motion motion = Motion::Subtle);

}  // namespace jalak::oled
