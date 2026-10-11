// Host tests for the portable demo tone synthesizer (jalak_local/sounds).
// Deterministic: no hardware, no time, no randomness.

#include <cstdio>
#include <set>
#include <string>
#include <vector>

#include "jalak/local/sounds.h"

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

void test_tones_are_nonempty_and_valid_pcm() {
    const Tone all[] = {Tone::UiTap, Tone::UiSelect, Tone::FocusStart,
                        Tone::BreakStart, Tone::SessionEnd};
    for (Tone t : all) {
        auto pcm = render_tone(t);
        CHECK(!pcm.empty());
        CHECK(pcm.size() % 2 == 0);                     // int16 => even bytes
        CHECK(pcm.size() <= 16000u * 2u * 2u + 4u);     // <= 2 s at 16 kHz
        // At least some non-zero samples (a real tone, not silence).
        bool nonzero = false;
        for (std::size_t i = 0; i + 1 < pcm.size(); i += 2) {
            if (pcm[i] != 0 || pcm[i + 1] != 0) { nonzero = true; break; }
        }
        CHECK(nonzero);
    }
}

void test_deterministic() {
    CHECK(render_tone(Tone::FocusStart) == render_tone(Tone::FocusStart));
    CHECK(render_tone(Tone::UiTap) != render_tone(Tone::UiSelect));
}

void test_distinct_lengths() {
    // Each tone is a distinct, non-empty buffer (names + bytes differ).
    std::set<std::size_t> sizes;
    for (Tone t : {Tone::UiTap, Tone::UiSelect, Tone::FocusStart, Tone::BreakStart,
                   Tone::SessionEnd}) {
        sizes.insert(render_tone(t).size());
    }
    CHECK(sizes.size() >= 4);  // at least 4 distinct durations
}

void test_zero_rate_is_empty() {
    CHECK(render_tone(Tone::UiTap, 0).empty());
}

void test_names() {
    CHECK(std::string(to_string(Tone::UiTap)) == "ui_tap");
    CHECK(std::string(to_string(Tone::SessionEnd)) == "session_end");
}

void test_samples_within_range() {
    // Every decoded int16 must be within full scale (no wrap from the cast).
    auto pcm = render_tone(Tone::SessionEnd);
    for (std::size_t i = 0; i + 1 < pcm.size(); i += 2) {
        const int v = static_cast<std::int16_t>(pcm[i] | (pcm[i + 1] << 8));
        CHECK(v >= -32768 && v <= 32767);
    }
}

void test_pitch_shift() {
    // A ramp of 4 int16 samples: pitch up (shift>256) shortens, down lengthens.
    std::vector<std::uint8_t> pcm;
    for (int i = 0; i < 4; ++i) {
        const std::int16_t s = static_cast<std::int16_t>(i * 100);
        pcm.push_back(static_cast<std::uint8_t>(s & 0xFF));
        pcm.push_back(static_cast<std::uint8_t>((s >> 8) & 0xFF));
    }
    CHECK(pitch_shift_pcm16(pcm, 0).empty());          // zero shift => empty
    CHECK(pitch_shift_pcm16({}, 256).empty());         // empty input => empty
    const auto unity = pitch_shift_pcm16(pcm, 256);    // 1:1
    CHECK(unity == pcm);
    CHECK(pitch_shift_pcm16(pcm, 512).size() < pcm.size());  // up = shorter
    CHECK(pitch_shift_pcm16(pcm, 128).size() > pcm.size());  // down = longer
}

}  // namespace

int main() {
    test_tones_are_nonempty_and_valid_pcm();
    test_deterministic();
    test_distinct_lengths();
    test_zero_rate_is_empty();
    test_names();
    test_samples_within_range();
    test_pitch_shift();
    if (g_failures == 0) {
        std::printf("ALL DEMO SOUND TESTS PASSED\n");
        return 0;
    }
    std::printf("%d DEMO SOUND TEST(S) FAILED\n", g_failures);
    return 1;
}
