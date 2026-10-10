#include "jalak/oled_codec.h"

#include <algorithm>
#include <cstddef>

namespace jalak {

namespace {

OledTransaction cmd(std::vector<std::uint8_t> bytes) {
    OledTransaction t;
    t.control = 0x00;  // Co=0, D/C#=0 -> command stream
    t.payload = std::move(bytes);
    return t;
}

int pages_for(int height) { return (height + 7) / 8; }

}  // namespace

bool oled_frame_size_ok(const OledPanel& panel, std::size_t byte_count) {
    if (panel.width <= 0 || panel.height <= 0) return false;
    const std::size_t expect =
        static_cast<std::size_t>(panel.width) * static_cast<std::size_t>(pages_for(panel.height));
    return byte_count == expect;
}

std::vector<OledTransaction> oled_init_sequence(const OledPanel& panel) {
    std::vector<OledTransaction> out;
    // Common init for SSD1306/SH1106 (both accept the SSD1306 subset).
    // Display OFF, clock, multiplex, offset, start line, charge pump, memory mode,
    // segment remap, COM scan direction, contrast, pre-charge, COM pins, on.
    out.push_back(cmd({0xAE}));                // display off
    out.push_back(cmd({0xD5, 0x80}));          // clock div / osc freq
    out.push_back(cmd({0xA8, static_cast<std::uint8_t>(panel.height - 1)}));  // multiplex
    out.push_back(cmd({0xD3, 0x00}));          // display offset
    out.push_back(cmd({0x40}));                // start line 0
    out.push_back(cmd({0x8D, 0x14}));          // charge pump on (SSD1306)
    out.push_back(cmd({0x20, 0x00}));          // memory addressing: horizontal
    // Segment remap / COM scan: rotate_180 flips both.
    out.push_back(cmd({static_cast<std::uint8_t>(panel.rotate_180 ? 0xA0 : 0xA1)}));
    out.push_back(cmd({static_cast<std::uint8_t>(panel.rotate_180 ? 0xC0 : 0xC8)}));
    out.push_back(cmd({0xDA, static_cast<std::uint8_t>(panel.height > 32 ? 0x12 : 0x02)}));  // COM pins config
    out.push_back(cmd({0x81, 0x7F}));          // contrast
    out.push_back(cmd({0xD9, 0xF1}));          // pre-charge
    out.push_back(cmd({0xDB, 0x40}));          // VCOMH
    out.push_back(cmd({0xA4}));                // entire display on: resume RAM
    out.push_back(cmd({0xA6}));                // normal (not inverted)
    out.push_back(cmd({0xAF}));                // display on
    return out;
}

std::vector<OledTransaction> oled_full_frame(const OledPanel& panel,
                                             const std::vector<std::uint8_t>& page_bytes) {
    std::vector<OledTransaction> out;
    if (!oled_frame_size_ok(panel, page_bytes.size())) return out;

    const int pages = pages_for(panel.height);
    const int col_base = panel.column_offset;

    for (int page = 0; page < pages; ++page) {
        // Column/page addressing. SH1106 uses per-page column set commands;
        // SSD1306 also accepts them, so we use the portable form for both.
        const int col = col_base;
        out.push_back(cmd({static_cast<std::uint8_t>(0xB0 | (page & 0x07))}));      // page
        out.push_back(cmd({static_cast<std::uint8_t>(0x00 | (col & 0x0F))}));       // low col
        out.push_back(cmd({static_cast<std::uint8_t>(0x10 | ((col >> 4) & 0x0F))})); // high col

        // Data transaction for this page's row of bytes.
        OledTransaction data;
        data.control = 0x40;  // Co=0, D/C#=1 -> data stream
        data.payload.assign(page_bytes.begin() + static_cast<std::size_t>(page) * panel.width,
                            page_bytes.begin() + static_cast<std::size_t>(page + 1) * panel.width);
        out.push_back(std::move(data));
    }
    return out;
}

}  // namespace jalak
