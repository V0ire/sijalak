#include "idf_mock.h"
#include "jalak/hal/i2s_audio_device.h"
#include "jalak/hal/sd_card.h"
#include <iostream>
#include <cstdio>
#ifndef JALAK_TEST_TMPDIR
#define JALAK_TEST_TMPDIR ".buildtmp"
#endif
#define CHECK(expr) do { if (!(expr)) { std::cerr << __LINE__ << ": " #expr "\n"; return 1; } } while (0)
int main(int argc, char** argv) {
    CHECK(argc == 2);
    using namespace jalak::hal;
    using namespace idf_mock;
    const I2sPins tx_only{4, 5, 6, -1, -1, -1};
    if (std::string(argv[1]) == "audio") {
        {
            I2sAudioDevice audio;
            CHECK(audio.begin(tx_only));
            CHECK(live == 1 && rx_calls == 0 && tx_rate == 24000);
            CHECK(!audio.start_capture());
            CHECK(audio.start()); CHECK(audio.play({0, 0, 1, 0})); CHECK(audio.stop());
        }
        CHECK(live == 0); reset();
        {
            fail_init_at = 1;
            I2sAudioDevice audio;
            CHECK(!audio.begin(tx_only)); CHECK(live == 0);
            fail_init_at = 0;
            CHECK(audio.begin(tx_only));
        }
        CHECK(live == 0);
        reset();
        {
            I2sAudioDevice audio;
            for (auto rate : {0u, 15999u, 16001u, 23999u, 24001u}) {
                CHECK(!audio.begin(tx_only, rate)); CHECK(live == 0);
            }
            CHECK(!audio.begin({4, 5, 6, -1, 5, -1}, 16000)); CHECK(live == 0);
            CHECK(!audio.begin({-1, 5, 6, -1, -1, -1}, 16000)); CHECK(live == 0);
            CHECK(audio.begin(tx_only, 16000)); CHECK(tx_rate == 16000 && rx_calls == 0);
            CHECK(!audio.begin(tx_only)); CHECK(live == 1);
            CHECK(audio.stop()); CHECK(audio.start()); CHECK(!audio.start());
            CHECK(!audio.play({})); CHECK(!audio.play({1})); CHECK(!audio.play({1, 2, 3}));
            CHECK(audio.play({1, 2}));
            fail_write = true; CHECK(!audio.play({1, 2})); fail_write = false;
            short_write = true; CHECK(!audio.play({1, 2})); short_write = false;
            fail_disable = true; CHECK(!audio.stop()); CHECK(audio.is_playing());
            fail_disable = false; CHECK(audio.stop()); CHECK(!audio.is_playing());
        }
        CHECK(live == 0); reset();
        // Failure at every allocation/init stage must release all owned channels.
        for (int stage = 1; stage <= 4; ++stage) {
            I2sAudioDevice audio;
            fail_new_at = stage <= 2 ? new_calls + stage : 0;
            fail_init_at = stage > 2 ? init_calls + stage - 2 : 0;
            CHECK(!audio.begin({4, 5, 6, 7, 8, 9})); CHECK(live == 0);
            fail_new_at = fail_init_at = 0;
        }
        {
            I2sAudioDevice audio;
            CHECK(audio.begin({4, 5, 6, 7, 8, 9})); CHECK(live == 2);
            CHECK(audio.start_capture()); CHECK(!audio.start());
            CHECK(audio.stop_capture().empty()); CHECK(!audio.is_capturing());
        }
        CHECK(live == 0);
    } else {
        {
            SdCard sd;
            CHECK(sd.begin({10, 11, 12, 13}));
            CHECK(host_seen == SPI2_HOST);
            sd.end(); CHECK(!sd.mounted());
            CHECK(unmount_calls == 1 && free_calls == 1 && !bus_owned);
            sd.end(); CHECK(unmount_calls == 1);
        }
        reset();
        {
            mount_fail = true;
            SdCard sd;
            CHECK(!sd.begin({10, 11, 12, 13})); CHECK(!bus_owned);
            mount_fail = false; CHECK(sd.begin({10, 11, 12, 13}));
            sd.end(); CHECK(!bus_owned);
        }
        reset();
        {
            SdCard sd;
            CHECK(!sd.begin({10, 11, 12, 13}, nullptr));
            CHECK(!sd.begin({10, 11, 12, 13}, ""));
            CHECK(!sd.begin({10, 11, 12, 13}, "relative"));
            CHECK(!sd.begin({10, 11, 12, 13}, ("/" + std::string(31, 'a')).c_str()));
            CHECK(!sd.begin({-1, 11, 12, 13}));
            CHECK(!sd.begin({10, 11, 12, 13, SPI1_HOST})); CHECK(bus_init_calls == 0);
            CHECK(sd.begin({10, 11, 12, 13}, ("/" + std::string(30, 'a')).c_str()));
            CHECK(sd.begin({10, 11, 12, 13})); CHECK(bus_init_calls == 1);
            unmount_fail = true; sd.end(); CHECK(sd.mounted() && bus_owned);
            unmount_fail = false; sd.end(); CHECK(!sd.mounted() && !bus_owned);
            CHECK(sd.begin({10, 11, 12, 13, SPI3_HOST})); CHECK(host_seen == SPI3_HOST);
            // Destructor must unmount the actual card and release this bus.
        }
        CHECK(!bus_owned); CHECK(free_calls == 2);
        reset();
        {
            SdCard sd;
            fail_bus_init = true;
            CHECK(!sd.begin({10, 11, 12, 13})); CHECK(!bus_owned && free_calls == 0);
            fail_bus_init = false; CHECK(sd.begin({10, 11, 12, 13}));
            const std::string path = JALAK_TEST_TMPDIR "/sd-read-fixture.bin";
            FILE* file = std::fopen(path.c_str(), "wb"); CHECK(file != nullptr);
            const std::uint8_t bytes[] = {0, 1, 2, 3, 4, 5, 6, 7};
            CHECK(std::fwrite(bytes, 1, sizeof bytes, file) == sizeof bytes);
            CHECK(std::fclose(file) == 0);
            CHECK(sd.exists(path)); CHECK(sd.read_file(path, 0).empty());
            CHECK(sd.read_file(path, 7).empty()); CHECK(sd.read_file(path, 8).size() == 8);
            CHECK(sd.read_file(path, 9) == std::vector<std::uint8_t>(bytes, bytes + 8));
            CHECK(sd.read_file(path + ".missing").empty());
            sd.end(); CHECK(!sd.exists(path) && sd.read_file(path).empty());
        }
        CHECK(!bus_owned);
    }
    std::cout << argv[1] << " real ESP_PLATFORM body / mock IDF boundary passed\n";
}
