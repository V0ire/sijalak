#pragma once
// Portable, hardware-independent eye/expression model for Si Jalak Bedas.
// No ESP-IDF or driver headers here so this compiles on a host.

#include <array>
#include <cstdint>

namespace jalak {

// The physical panel is a 1.3" 128x64 1-bit OLED. The controller (SH1106 vs
// SSD1306) is NOT known for the owned module, so nothing here assumes one.
// We render into a neutral 1-bit framebuffer that a driver later pushes.
struct PanelGeometry {
    int width = 128;
    int height = 64;
};

// A single eye is drawn as a filled rounded region. Expressions change the
// aperture (openness), vertical offset, and lid/corner shape.
struct EyeParams {
    int cx = 0;          // center x
    int cy = 0;          // center y
    int aperture_pct = 100;  // 0 = closed, 100 = fully open
    int width = 22;      // nominal half-open eye width
    int height = 20;     // nominal fully-open eye height
    int brow_tilt = 0;   // negative = angled down/out (annoyed), positive = up (curious)
    int lid_slope = 0;   // positive raises the outer lid corner
};

enum class Mood {
    Neutral,
    Happy,
    Curious,
    Mischievous,
    Sleepy,
    Surprised,
    Annoyed,
    Thinking,
    Listening,
    Speaking,
    Error,
};

struct Expression {
    Mood mood = Mood::Neutral;
    int left_aperture_pct = 100;
    int right_aperture_pct = 100;
    int look_dx = 0;   // pupil/eye region horizontal shift, px
    int look_dy = 0;   // vertical shift, px
    int brow_tilt = 0;
    bool left_wink = false;
    bool right_wink = false;
};

// Map a mood to base expression parameters. This is pure logic and unit-tested.
Expression expression_for(Mood mood);

// Clamp/normalize a possibly wild expression into valid panel bounds.
Expression clamp_expression(const Expression& in, const PanelGeometry& geo);

}  // namespace jalak
