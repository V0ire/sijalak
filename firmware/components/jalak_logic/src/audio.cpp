#include "jalak/audio.h"

#include <string>

namespace jalak {

std::size_t canonical_wav_bytes(std::size_t pcm_bytes) { return pcm_bytes + 44; }

std::size_t pcm_bytes_for_ms(std::uint32_t sample_rate, std::uint16_t bytes_per_sample,
                            int ms) {
    if (ms <= 0) return 0;
    // bytes = rate * bytes_per_sample * ms / 1000
    const std::uint64_t v =
        static_cast<std::uint64_t>(sample_rate) * bytes_per_sample *
        static_cast<std::uint64_t>(ms) / 1000u;
    return static_cast<std::size_t>(v);
}

std::size_t input_cap_bytes() {
    // 16000 Hz * 2 bytes * 8 s = 256000 + 44 header
    return canonical_wav_bytes(pcm_bytes_for_ms(16000, 2, 8000));
}

std::size_t output_cap_bytes() {
    // 24000 Hz * 2 bytes * 15 s = 720000 + 44 header
    return canonical_wav_bytes(pcm_bytes_for_ms(24000, 2, 15000));
}

bool validate_capture_size(std::size_t bytes, std::string* reason) {
    if (bytes == 0) {
        if (reason) *reason = "empty capture";
        return false;
    }
    if (bytes < 45) {
        if (reason) *reason = "shorter than a WAV header";
        return false;
    }
    if (bytes > input_cap_bytes()) {
        if (reason) *reason = "capture exceeds 8 s canonical cap";
        return false;
    }
    if (reason) *reason = "ok";
    return true;
}

}  // namespace jalak
