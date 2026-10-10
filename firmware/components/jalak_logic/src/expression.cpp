#include "jalak/expression.h"

namespace jalak {

namespace {
int clamp_int(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}
}  // namespace

Expression expression_for(Mood mood) {
    Expression e;
    e.mood = mood;
    switch (mood) {
        case Mood::Neutral:
            e.left_aperture_pct = 100;
            e.right_aperture_pct = 100;
            break;
        case Mood::Happy:
            // Eyes narrowed upward; the "playful, not creepy" default.
            e.left_aperture_pct = 70;
            e.right_aperture_pct = 70;
            e.brow_tilt = 6;
            break;
        case Mood::Curious:
            e.left_aperture_pct = 100;
            e.right_aperture_pct = 80;
            e.look_dx = 3;
            e.brow_tilt = 4;
            break;
        case Mood::Mischievous:
            // One eye squints: a wink, not a scowl. Humor is aimed at the
            // robot/situation, never at the reader.
            e.left_aperture_pct = 100;
            e.right_aperture_pct = 45;
            e.right_wink = true;
            e.brow_tilt = -3;
            e.look_dx = 2;
            break;
        case Mood::Sleepy:
            e.left_aperture_pct = 30;
            e.right_aperture_pct = 30;
            e.look_dy = 2;
            break;
        case Mood::Surprised:
            e.left_aperture_pct = 120;  // clamped later
            e.right_aperture_pct = 120;
            e.brow_tilt = 8;
            break;
        case Mood::Annoyed:
            e.left_aperture_pct = 60;
            e.right_aperture_pct = 60;
            e.brow_tilt = -7;
            break;
        case Mood::Thinking:
            e.left_aperture_pct = 85;
            e.right_aperture_pct = 85;
            e.look_dx = -4;
            e.look_dy = -2;
            break;
        case Mood::Listening:
            e.left_aperture_pct = 110;
            e.right_aperture_pct = 110;
            e.brow_tilt = 5;
            break;
        case Mood::Speaking:
            e.left_aperture_pct = 85;
            e.right_aperture_pct = 85;
            e.look_dy = 1;
            break;
        case Mood::Error:
            e.left_aperture_pct = 50;
            e.right_aperture_pct = 50;
            e.brow_tilt = -5;
            e.look_dy = 3;
            break;
    }
    return e;
}

Expression clamp_expression(const Expression& in, const PanelGeometry& geo) {
    Expression out = in;
    out.left_aperture_pct = clamp_int(out.left_aperture_pct, 0, 100);
    out.right_aperture_pct = clamp_int(out.right_aperture_pct, 0, 100);
    // Keep the eyes inside the panel with a small margin.
    const int max_dx = geo.width / 4;
    const int max_dy = geo.height / 4;
    out.look_dx = clamp_int(out.look_dx, -max_dx, max_dx);
    out.look_dy = clamp_int(out.look_dy, -max_dy, max_dy);
    out.brow_tilt = clamp_int(out.brow_tilt, -10, 10);
    return out;
}

}  // namespace jalak
