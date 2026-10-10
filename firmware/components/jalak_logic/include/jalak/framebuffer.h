#pragma once
// 1-bit monochrome framebuffer + primitives. Hardware-independent.

#include <cstdint>
#include <vector>

#include "jalak/expression.h"

namespace jalak {

class Framebuffer {
public:
    explicit Framebuffer(PanelGeometry geo = {});

    PanelGeometry geometry() const { return geo_; }
    int width() const { return geo_.width; }
    int height() const { return geo_.height; }

    void clear(bool value = false);
    bool get(int x, int y) const;
    void set(int x, int y, bool value = true);

    // Filled axis-aligned rectangle, clipped to panel bounds.
    void fill_rect(int x, int y, int w, int h, bool value = true);

    // Filled ellipse centered at (cx, cy) with radii (rx, ry), clipped.
    // Used for eye apertures and rounded character features.
    void fill_ellipse(int cx, int cy, int rx, int ry, bool value = true);

    // Filled rectangle with rounded corners (corner radius r), clipped.
    void fill_rounded_rect(int x, int y, int w, int h, int r, bool value = true);

    // Raw bytes in page format (8 vertical pixels per byte, LSB = top row),
    // the layout an SH1106/SSD1306 driver typically consumes. Length ==
    // width * ceil(height/8). This is a byte container, not a panel command.
    std::vector<std::uint8_t> to_page_bytes() const;

private:
    PanelGeometry geo_;
    std::vector<std::uint8_t> bits_;  // 1 byte per pixel for simplicity
};

// Render two eyes for the given expression into the framebuffer. The face is
// centered horizontally with a small gap. Returns the number of pixels set.
int draw_face(Framebuffer& fb, const Expression& expr);

// Draw a simple mouth hint for a mood (small, deliberately minimal).
int draw_mouth(Framebuffer& fb, Mood mood);

}  // namespace jalak
