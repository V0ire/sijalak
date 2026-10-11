// Host tests for the portable clip player (jalak_local/clip).

#include <cstdio>
#include <vector>

#include "jalak/framebuffer.h"
#include "jalak/local/clip.h"

namespace {
int g_failures = 0;
#define CHECK(cond)                                                     \
    do {                                                                \
        if (!(cond)) {                                                  \
            std::printf("FAIL: %s (line %d)\n", #cond, __LINE__);       \
            ++g_failures;                                               \
        }                                                               \
    } while (0)

using namespace jalak::local;
using jalak::Framebuffer;

std::vector<std::uint8_t> frame_with(std::size_t px_index) {
    std::vector<std::uint8_t> f(kFrameBytes, 0);
    // Set one pixel: page = y/8, byte = page*128 + x, bit = y%8.
    const std::size_t x = px_index % kFrameW;
    const std::size_t y = px_index / kFrameW;
    f[(y / 8) * kFrameW + x] |= static_cast<std::uint8_t>(1u << (y % 8));
    return f;
}

void test_unpack_size_mismatch_fails_closed() {
    std::vector<std::uint8_t> blob(kFrameBytes * 2 + 1, 0);  // not a multiple
    CHECK(unpack_frames(blob, 2).empty());
    CHECK(unpack_frames({}, 0).empty());
    CHECK(unpack_frames(std::vector<std::uint8_t>(kFrameBytes, 0), 1).size() == 1);
}

void test_load_rejects_wrong_frame_size() {
    ClipPlayer p;
    Clip bad;
    bad.name = "bad";
    bad.frames = {std::vector<std::uint8_t>(10, 0)};  // too short
    CHECK(!p.load(bad));
    CHECK(!p.loaded());
}

void test_tick_advances_and_wraps() {
    ClipPlayer p;
    Clip c;
    c.name = "two";
    c.frame_delay_ms = 100;
    c.frames = {frame_with(0), frame_with(1)};
    CHECK(p.load(c));
    CHECK(p.frame_count() == 2);

    CHECK(p.tick(1000));          // first tick starts playback
    CHECK(p.frame_index() == 0);
    CHECK(!p.tick(1050));         // < delay: no change
    CHECK(p.tick(1100));          // exactly delay: advance
    CHECK(p.frame_index() == 1);
    CHECK(p.tick(1200));
    CHECK(p.frame_index() == 0);  // wrapped
}

void test_large_delta_is_bounded_and_no_overflow() {
    ClipPlayer p;
    Clip c;
    c.frame_delay_ms = 100;
    c.frames = {frame_with(0), frame_with(1), frame_with(2)};
    CHECK(p.load(c));
    p.tick(0);
    // A huge delta must not loop per frame and must not overflow.
    CHECK(p.tick(0xFFFFFFFFFFFFFFFFull));
    CHECK(p.frame_index() < 3);
}

void test_backwards_time_ignored() {
    ClipPlayer p;
    Clip c;
    c.frame_delay_ms = 100;
    c.frames = {frame_with(0), frame_with(1)};
    CHECK(p.load(c));
    p.tick(5000);
    const auto before = p.frame_index();
    CHECK(!p.tick(4000));  // backwards: ignored
    CHECK(p.frame_index() == before);
}

void test_page_bytes_and_draw() {
    ClipPlayer p;
    Clip c;
    c.frame_delay_ms = 100;
    c.frames = {frame_with(5)};  // pixel (5,0)
    CHECK(p.load(c));
    p.tick(0);

    std::vector<std::uint8_t> bytes;
    CHECK(p.page_bytes(bytes));
    CHECK(bytes.size() == kFrameBytes);

    Framebuffer fb({kFrameW, kFrameH});
    CHECK(p.draw(fb));
    CHECK(fb.get(5, 0));
    CHECK(!fb.get(6, 0));
}

void test_clear() {
    ClipPlayer p;
    Clip c;
    c.frame_delay_ms = 100;
    c.frames = {frame_with(0)};
    p.load(c);
    CHECK(p.loaded());
    p.clear();
    CHECK(!p.loaded());
    std::vector<std::uint8_t> b;
    CHECK(!p.page_bytes(b));
}

}  // namespace

int main() {
    test_unpack_size_mismatch_fails_closed();
    test_load_rejects_wrong_frame_size();
    test_tick_advances_and_wraps();
    test_large_delta_is_bounded_and_no_overflow();
    test_backwards_time_ignored();
    test_page_bytes_and_draw();
    test_clear();
    if (g_failures == 0) {
        std::printf("ALL CLIP PLAYER TESTS PASSED\n");
        return 0;
    }
    std::printf("%d CLIP PLAYER TEST(S) FAILED\n", g_failures);
    return 1;
}
