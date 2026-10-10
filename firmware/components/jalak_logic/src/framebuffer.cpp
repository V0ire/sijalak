#include "jalak/framebuffer.h"

#include <algorithm>
#include <cmath>

namespace jalak {

namespace {
int clampi(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}
}  // namespace

Framebuffer::Framebuffer(PanelGeometry geo) : geo_(geo) {
    // Guard against nonsense geometry supplied by a misconfigured caller.
    if (geo_.width <= 0) geo_.width = 128;
    if (geo_.height <= 0) geo_.height = 64;
    bits_.assign(static_cast<std::size_t>(geo_.width) * geo_.height, 0);
}

void Framebuffer::clear(bool value) {
    std::fill(bits_.begin(), bits_.end(), value ? 1 : 0);
}

bool Framebuffer::get(int x, int y) const {
    if (x < 0 || y < 0 || x >= geo_.width || y >= geo_.height) return false;
    return bits_[static_cast<std::size_t>(y) * geo_.width + x] != 0;
}

void Framebuffer::set(int x, int y, bool value) {
    if (x < 0 || y < 0 || x >= geo_.width || y >= geo_.height) return;
    bits_[static_cast<std::size_t>(y) * geo_.width + x] = value ? 1 : 0;
}

void Framebuffer::fill_rect(int x, int y, int w, int h, bool value) {
    if (w <= 0 || h <= 0) return;
    const int x0 = clampi(x, 0, geo_.width);
    const int y0 = clampi(y, 0, geo_.height);
    const int x1 = clampi(x + w, 0, geo_.width);
    const int y1 = clampi(y + h, 0, geo_.height);
    for (int yy = y0; yy < y1; ++yy) {
        for (int xx = x0; xx < x1; ++xx) {
            set(xx, yy, value);
        }
    }
}

void Framebuffer::fill_ellipse(int cx, int cy, int rx, int ry, bool value) {
    if (rx <= 0 || ry <= 0) return;
    const double rx2 = static_cast<double>(rx) * rx;
    const double ry2 = static_cast<double>(ry) * ry;
    const int x0 = std::max(0, cx - rx);
    const int x1 = std::min(geo_.width - 1, cx + rx);
    const int y0 = std::max(0, cy - ry);
    const int y1 = std::min(geo_.height - 1, cy + ry);
    for (int yy = y0; yy <= y1; ++yy) {
        const double dy = (yy - cy);
        for (int xx = x0; xx <= x1; ++xx) {
            const double dx = (xx - cx);
            if ((dx * dx) / rx2 + (dy * dy) / ry2 <= 1.0) {
                set(xx, yy, value);
            }
        }
    }
}

void Framebuffer::fill_rounded_rect(int x, int y, int w, int h, int r, bool value) {
    if (w <= 0 || h <= 0) return;
    if (r <= 0) {
        fill_rect(x, y, w, h, value);
        return;
    }
    r = std::min(r, std::min(w / 2, h / 2));
    // Middle band + top/bottom bands with rounded ends.
    fill_rect(x, y + r, w, h - 2 * r, value);
    fill_rect(x + r, y, w - 2 * r, r, value);
    fill_rect(x + r, y + h - r, w - 2 * r, r, value);
    // Corner circles.
    fill_ellipse(x + r, y + r, r, r, value);
    fill_ellipse(x + w - r - 1, y + r, r, r, value);
    fill_ellipse(x + r, y + h - r - 1, r, r, value);
    fill_ellipse(x + w - r - 1, y + h - r - 1, r, r, value);
}

std::vector<std::uint8_t> Framebuffer::to_page_bytes() const {
    const int pages = (geo_.height + 7) / 8;
    std::vector<std::uint8_t> out(static_cast<std::size_t>(geo_.width) * pages, 0);
    for (int page = 0; page < pages; ++page) {
        for (int x = 0; x < geo_.width; ++x) {
            std::uint8_t byte = 0;
            for (int bit = 0; bit < 8; ++bit) {
                const int y = page * 8 + bit;
                if (y >= geo_.height) break;
                if (get(x, y)) byte |= (1u << bit);
            }
            out[static_cast<std::size_t>(page) * geo_.width + x] = byte;
        }
    }
    return out;
}

int draw_face(Framebuffer& fb, const Expression& raw) {
    const PanelGeometry geo = fb.geometry();
    const Expression e = clamp_expression(raw, geo);
    fb.clear(false);

    const int eye_cy = geo.height / 2 - 2;
    const int gap = 8;                      // gap between eyes
    const int eye_rx = geo.width / 8;       // ~16 px on 128 wide
    const int eye_ry = eye_rx + 2;
    const int left_cx = geo.width / 2 - gap / 2 - eye_rx + e.look_dx;
    const int right_cx = geo.width / 2 + gap / 2 + eye_rx + e.look_dx;
    const int cy = eye_cy + e.look_dy;

    // Paint-only: this lambda only writes pixels and never touches the tally,
    // so the single post-pass scan below is the sole source of `painted`.
    auto paint_eye = [&](int cx, int aperture_pct, bool wink, int tilt) {
        if (wink || aperture_pct <= 0) {
            // A wink/closed eye is a horizontal line, softened by tilt.
            const int half = eye_rx;
            for (int dx = -half; dx <= half; ++dx) {
                const int y = cy + (tilt * dx) / std::max(1, half);
                fb.set(cx + dx, y, true);
            }
            return;
        }
        const int ry = std::max(1, (eye_ry * aperture_pct) / 100);
        for (int yy = cy - ry; yy <= cy + ry; ++yy) {
            for (int xx = cx - eye_rx; xx <= cx + eye_rx; ++xx) {
                const bool inside =
                    (static_cast<double>((xx - cx) * (xx - cx)) /
                         (static_cast<double>(eye_rx) * eye_rx) +
                     static_cast<double>((yy - cy) * (yy - cy)) /
                         (static_cast<double>(ry) * ry)) <= 1.0;
                if (inside) fb.set(xx, yy, true);
            }
        }
    };

    paint_eye(left_cx, e.left_aperture_pct, e.left_wink, e.brow_tilt);
    paint_eye(right_cx, e.right_aperture_pct, e.right_wink, e.brow_tilt);

    // Single authoritative count of the finished frame.
    int painted = 0;

    for (int y = 0; y < geo.height; ++y) {
        for (int x = 0; x < geo.width; ++x) {
            if (fb.get(x, y)) ++painted;
        }
    }
    return painted;
}

int draw_mouth(Framebuffer& fb, Mood mood) {
    const PanelGeometry geo = fb.geometry();
    const int y = geo.height - 12;
    const int cx = geo.width / 2;
    // Count-on-write contract: `painted` counts pixels this call sets that were
    // previously unset. This function must NOT rescan the full frame (unlike
    // draw_face) or pixels drawn by other calls would be double counted.
    int painted = 0;
    auto put = [&](int x, int yy) {
        if (!fb.get(x, yy)) {
            fb.set(x, yy, true);
            ++painted;
        }
    };
    switch (mood) {
        case Mood::Happy:
        case Mood::Mischievous:
            // Small upward arc.
            for (int dx = -6; dx <= 6; ++dx) {
                const int dy = (dx * dx) / 9;  // parabola
                put(cx + dx, y - dy + 2);
            }
            break;
        case Mood::Surprised:
            fb.fill_ellipse(cx, y, 3, 4, true);
            // Count pixels this call newly set, consistent with the put() path.
            for (int yy = y - 4; yy <= y + 4; ++yy)
                for (int xx = cx - 3; xx <= cx + 3; ++xx) {
                    const double dx = xx - cx, dy = yy - y;
                    if ((dx * dx) / 9.0 + (dy * dy) / 16.0 <= 1.0 &&
                        fb.get(xx, yy))
                        ++painted;
                }
            break;
        case Mood::Annoyed:
        case Mood::Error:
            // Small downward arc.
            for (int dx = -6; dx <= 6; ++dx) {
                const int dy = (dx * dx) / 9;
                put(cx + dx, y + dy - 2);
            }
            break;
        default:
            // Neutral: short flat line.
            for (int dx = -4; dx <= 4; ++dx) put(cx + dx, y);
            break;
    }
    return painted;
}

}  // namespace jalak
