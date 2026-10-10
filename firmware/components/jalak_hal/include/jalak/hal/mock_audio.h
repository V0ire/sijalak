#pragma once
// Mock audio source/sink for host tests and simulation. No I2S involved.

#include <functional>
#include <vector>

#include "jalak/audio.h"

namespace jalak::mock {

class MockAudioSink : public AudioSink {
public:
    bool start() override { started_ = true; return true; }
    bool stop() override { started_ = false; playing_ = false; return true; }
    bool play(const std::vector<std::uint8_t>& pcm) override {
        if (!started_) return false;
        last_playback_ = pcm;
        playing_ = true;
        ++play_count_;
        return true;
    }
    bool is_playing() const override { return playing_; }

    // Test helper: simulate playback finishing.
    void finish_playback() { playing_ = false; }

    int play_count() const { return play_count_; }
    const std::vector<std::uint8_t>& last_playback() const { return last_playback_; }

private:
    bool started_ = false;
    bool playing_ = false;
    int play_count_ = 0;
    std::vector<std::uint8_t> last_playback_;
};

class MockAudioSource : public AudioSource {
public:
    // Scripted capture bytes returned by stop_capture().
    void set_next_capture(std::vector<std::uint8_t> bytes) {
        next_capture_ = std::move(bytes);
    }
    bool start_capture() override {
        if (capturing_) return false;  // no overlapping capture
        capturing_ = true;
        return true;
    }
    std::vector<std::uint8_t> stop_capture() override {
        capturing_ = false;
        auto out = next_capture_;
        next_capture_.clear();
        return out;
    }
    bool is_capturing() const override { return capturing_; }

private:
    bool capturing_ = false;
    std::vector<std::uint8_t> next_capture_;
};

}  // namespace jalak::mock
