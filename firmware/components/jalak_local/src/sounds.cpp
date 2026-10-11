#include "jalak/local/sounds.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace jalak::local {

namespace {

// A single tone segment: frequency (Hz), duration (ms), peak amplitude (0..1).
struct Segment {
    double freq_hz;
    int duration_ms;
    double amplitude;
};

std::vector<Segment> segments_for(Tone tone) {
    switch (tone) {
        case Tone::UiTap:
            return {{880.0, 45, 0.25}};
        case Tone::UiSelect:
            return {{660.0, 60, 0.30}, {990.0, 70, 0.30}};
        case Tone::FocusStart:
            return {{523.25, 120, 0.35}, {659.25, 120, 0.35}, {783.99, 180, 0.35}};
        case Tone::BreakStart:
            return {{783.99, 140, 0.30}, {587.33, 200, 0.30}};
        case Tone::SessionEnd:
            return {{659.25, 160, 0.35}, {523.25, 160, 0.35}, {392.00, 260, 0.35}};
    }
    return {{440.0, 80, 0.25}};
}

// Cap total tone length so a bug can never allocate an unbounded buffer.
constexpr int kMaxTotalMs = 2000;

}  // namespace

const char* to_string(Tone tone) {
    switch (tone) {
        case Tone::UiTap: return "ui_tap";
        case Tone::UiSelect: return "ui_select";
        case Tone::FocusStart: return "focus_start";
        case Tone::BreakStart: return "break_start";
        case Tone::SessionEnd: return "session_end";
    }
    return "unknown";
}

std::vector<std::uint8_t> render_tone(Tone tone, std::uint32_t rate) {
    std::vector<std::uint8_t> out;
    if (rate == 0) return out;

    int total_ms = 0;
    const auto segs = segments_for(tone);
    for (const auto& s : segs) total_ms += s.duration_ms;
    if (total_ms > kMaxTotalMs) return out;  // refuse absurd configs

    out.reserve(static_cast<std::size_t>(rate) * static_cast<std::size_t>(total_ms) / 1000 * 2);

    for (const auto& s : segs) {
        const int samples = static_cast<int>(static_cast<std::uint64_t>(rate) * s.duration_ms / 1000);
        for (int i = 0; i < samples; ++i) {
            const double t = static_cast<double>(i) / static_cast<double>(rate);
            // Linear fade in/out over the first/last 5 ms to avoid clicks.
            const double fade_n = static_cast<double>(std::min(samples, static_cast<int>(rate * 5 / 1000)));
            double env = 1.0;
            if (fade_n > 0) {
                const double fin = static_cast<double>(i) / fade_n;
                const double fout = static_cast<double>(samples - 1 - i) / fade_n;
                env = std::min({1.0, fin, fout});
            }
            const double v = std::sin(2.0 * M_PI * s.freq_hz * t) * s.amplitude * env;
            const int iv = static_cast<int>(std::lround(std::clamp(v, -1.0, 1.0) * 32767.0));
            const std::int16_t s16 = static_cast<std::int16_t>(iv);
            out.push_back(static_cast<std::uint8_t>(s16 & 0xFF));
            out.push_back(static_cast<std::uint8_t>((s16 >> 8) & 0xFF));
        }
    }
    return out;
}

std::vector<std::uint8_t> pitch_shift_pcm16(const std::vector<std::uint8_t>& pcm,
                                            unsigned shift_256) {
    std::vector<std::uint8_t> out;
    if (shift_256 == 0 || pcm.size() < 2) return out;
    const std::size_t in_samples = pcm.size() / 2;
    // Linear resample: output position p -> input index p*256/shift.
    const std::size_t out_samples = in_samples * 256u / shift_256;
    out.reserve(out_samples * 2);
    for (std::size_t p = 0; p < out_samples; ++p) {
        std::size_t src = p * 256u / shift_256;
        if (src >= in_samples) src = in_samples - 1;  // hold last sample (slow-down)
        out.push_back(pcm[src * 2]);
        out.push_back(pcm[src * 2 + 1]);
    }
    return out;
}

}  // namespace jalak::local
