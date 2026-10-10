#pragma once
// Audio framing + half-duplex rules. No I2S/IDF headers: this is the portable
// contract the real driver must satisfy, and what the mock harness exercises.

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace jalak {

struct AudioFormat {
    std::uint32_t sample_rate = 16000;  // canonical input
    std::uint16_t channels = 1;
    std::uint16_t bits_per_sample = 16;
    bool little_endian = true;
};

// Canonical-byte math. A 44-byte canonical WAV header plus PCM payload.
// These are the numbers the docs specify and the tests assert.
struct AudioLimits {
    int input_max_ms = 8000;    // 16000 Hz * 2 B * 1 ch * 8 s = 256000 + 44
    int output_max_ms = 15000;  // 24000 Hz * 2 B * 1 ch * 15 s = 720000 + 44
    std::size_t download_cap_bytes = 768 * 1024;
};

std::size_t canonical_wav_bytes(std::size_t pcm_bytes);
std::size_t pcm_bytes_for_ms(std::uint32_t sample_rate, std::uint16_t bytes_per_sample,
                            int ms);
std::size_t input_cap_bytes();
std::size_t output_cap_bytes();

// Validate that a captured buffer respects the canonical input cap. Returns
// false with a reason string when oversize/undersize/empty.
bool validate_capture_size(std::size_t bytes, std::string* reason);

// Interface the real I2S implementation must provide. The mock harness
// implements this without hardware.
class AudioSink {
public:
    virtual ~AudioSink() = default;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool play(const std::vector<std::uint8_t>& pcm) = 0;
    virtual bool is_playing() const = 0;
};

class AudioSource {
public:
    virtual ~AudioSource() = default;
    virtual bool start_capture() = 0;
    virtual std::vector<std::uint8_t> stop_capture() = 0;
    virtual bool is_capturing() const = 0;
};

}  // namespace jalak
