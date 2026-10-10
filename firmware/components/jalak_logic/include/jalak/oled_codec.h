#pragma once
// Portable OLED command/page encoder for SSD1306 and SH1106.
//
// This is pure logic: given a 1-bit page buffer it emits the exact I2C control
// byte sequences a driver would send. It is host-testable with no hardware, which
// is why it lives here (not in the IDF driver). The IDF driver (jalak_hal) is the
// only piece that touches i2c_master; it is UNVERIFIED until a board exists.
//
// Controller differences handled here:
//   * SSD1306 is 128x64, pages 0..7, column range 0..127.
//   * SH1106 has 132-column RAM; a 128-wide image needs a column offset (usually
//     2) via "set lower/higher column" commands.
//   * Both accept 0x40 as the "data follows" control byte and 0x00 for commands.

#include <cstdint>
#include <vector>

namespace jalak {

enum class OledControllerKind { Ssd1306, Sh1106 };

struct OledPanel {
    int width = 128;
    int height = 64;
    OledControllerKind controller = OledControllerKind::Ssd1306;
    int column_offset = 0;  // SH1106: 2 is typical for a 128-wide image
    bool rotate_180 = false;
};

// One I2C write transaction: a control byte followed by payload bytes.
struct OledTransaction {
    std::uint8_t control;
    std::vector<std::uint8_t> payload;
};

// Initialization command list for the panel. Each transaction has control 0x00
// (command stream). Deterministic and unit-tested.
std::vector<OledTransaction> oled_init_sequence(const OledPanel& panel);

// Page-address + data transactions that push a full 1-bit page buffer.
// `page_bytes` must be width * (height/8) bytes in SSD1306 page format.
// Throws nothing; returns an empty vector if the size is wrong.
std::vector<OledTransaction> oled_full_frame(const OledPanel& panel,
                                             const std::vector<std::uint8_t>& page_bytes);

// True if page_bytes matches width * ceil(height/8).
bool oled_frame_size_ok(const OledPanel& panel, std::size_t byte_count);

}  // namespace jalak
