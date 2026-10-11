#pragma once
// Portable demo sounds: a tiny PCM synthesizer for UI feedback plus a PCM
// player buffer abstraction. No ESP-IDF, no hardware — host-testable.
//
// The Mochi "pop" is a third-party asset loaded from SD (see tools/mochi_assets.py)
// and is NOT synthesized here. These tones are ORIGINAL (generated from math) so
// they carry no third-party rights and are safe to keep in the repository.
//
// Format matches the I2S TX path: 16-bit signed PCM, mono, 16 kHz.

#include <cstdint>
#include <vector>

namespace jalak::local {

enum class Tone {
    UiTap,     // short blip when moving a menu selection
    UiSelect,  // confirm / start
    FocusStart,
    BreakStart,
    SessionEnd,
};

// Sample rate of every tone and of the Mochi pop (matches the amp config).
constexpr std::uint32_t kAudioRateHz = 16000;

// Generate one tone as little-endian PCM16 mono samples. Deterministic: equal
// inputs give equal bytes, so tests can hash the output.
std::vector<std::uint8_t> render_tone(Tone tone, std::uint32_t rate = kAudioRateHz);

// Short human-readable name for logging/tests.
const char* to_string(Tone tone);

// Resample 16-bit mono PCM by an integer ratio/256 to vary pitch, mirroring the
// Mochi pop's ±8% random shift but deterministic here. Returns empty for an
// empty input or a zero shift. Output length is roughly len*256/shift.
std::vector<std::uint8_t> pitch_shift_pcm16(const std::vector<std::uint8_t>& pcm,
                                            unsigned shift_256);

}  // namespace jalak::local
