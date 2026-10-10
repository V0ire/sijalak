// Host tests for the portable OLED command/page encoder (SSD1306 + SH1106).
// No I2C, no hardware. Proves the byte sequences a real driver would send.

#include <cstdio>
#include <vector>

#include "jalak/oled_codec.h"

namespace {
int g_failures = 0;
#define CHECK(cond)                                                     \
    do {                                                                \
        if (!(cond)) {                                                  \
            std::printf("FAIL: %s (line %d)\n", #cond, __LINE__);       \
            ++g_failures;                                               \
        }                                                               \
    } while (0)

void test_frame_size_validation() {
    jalak::OledPanel p;
    CHECK(jalak::oled_frame_size_ok(p, 128u * 8u));   // 64/8 = 8 pages
    CHECK(!jalak::oled_frame_size_ok(p, 128u * 7u));
    CHECK(!jalak::oled_frame_size_ok(p, 0));
}

void test_init_sequence_has_essentials() {
    jalak::OledPanel p;
    p.controller = jalak::OledControllerKind::Ssd1306;
    auto seq = jalak::oled_init_sequence(p);
    CHECK(!seq.empty());
    // Every init transaction is a command stream (control 0x00).
    for (auto& t : seq) CHECK(t.control == 0x00);
    // First command must be display-off (0xAE); last must be display-on (0xAF).
    CHECK(seq.front().payload.size() >= 1 && seq.front().payload[0] == 0xAE);
    CHECK(seq.back().payload.size() >= 1 && seq.back().payload[0] == 0xAF);
}

void test_rotate_180_flips_remap_and_scan() {
    jalak::OledPanel normal;
    jalak::OledPanel flipped;
    flipped.rotate_180 = true;
    auto a = jalak::oled_init_sequence(normal);
    auto b = jalak::oled_init_sequence(flipped);
    auto has = [](const std::vector<jalak::OledTransaction>& s, std::uint8_t op) {
        for (auto& t : s)
            for (auto v : t.payload)
                if (v == op) return true;
        return false;
    };
    CHECK(has(a, 0xA1) && has(a, 0xC8));  // normal
    CHECK(has(b, 0xA0) && has(b, 0xC0));  // rotated 180
}

void test_full_frame_transactions() {
    jalak::OledPanel p;  // 128x64 SSD1306, column_offset 0
    std::vector<std::uint8_t> fb(128u * 8u, 0);
    fb[0] = 0xAB;  // page 0, column 0
    auto txs = jalak::oled_full_frame(p, fb);
    // Per page: 3 command txns (page, col low, col high) + 1 data txn = 4*8 = 32.
    CHECK(txs.size() == 32u);
    // First page's data transaction carries exactly 128 bytes and starts with 0xAB.
    // Order is: cmd(page), cmd(lowcol), cmd(highcol), data(page0).
    CHECK(txs[0].control == 0x00 && txs[0].payload[0] == 0xB0);  // page 0
    CHECK(txs[1].control == 0x00 && txs[1].payload[0] == 0x00);  // col low
    CHECK(txs[2].control == 0x00 && txs[2].payload[0] == 0x10);  // col high
    CHECK(txs[3].control == 0x40);                               // data
    CHECK(txs[3].payload.size() == 128u);
    CHECK(txs[3].payload[0] == 0xAB);
}

void test_sh1106_column_offset_applied() {
    jalak::OledPanel p;
    p.controller = jalak::OledControllerKind::Sh1106;
    p.column_offset = 2;
    std::vector<std::uint8_t> fb(128u * 8u, 0);
    auto txs = jalak::oled_full_frame(p, fb);
    // Low col = 2, high col = 0x10 | 0 for offset 2.
    CHECK(txs[1].payload[0] == 0x02);
    CHECK(txs[2].payload[0] == 0x10);
}

void test_bad_frame_size_returns_empty() {
    jalak::OledPanel p;
    std::vector<std::uint8_t> fb(100, 0);  // wrong size
    CHECK(jalak::oled_full_frame(p, fb).empty());
}

}  // namespace

int main() {
    test_frame_size_validation();
    test_init_sequence_has_essentials();
    test_rotate_180_flips_remap_and_scan();
    test_full_frame_transactions();
    test_sh1106_column_offset_applied();
    test_bad_frame_size_returns_empty();
    if (g_failures == 0) {
        std::printf("ALL OLED CODEC HOST TESTS PASSED\n");
        return 0;
    }
    std::printf("%d OLED CODEC TEST(S) FAILED\n", g_failures);
    return 1;
}
