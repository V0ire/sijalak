# Si Jalak Bedas — firmware (ESP-IDF)

Offline-testable firmware for the Si Jalak Bedas literacy companion.

## Layout

```
firmware/
  CMakeLists.txt          # ESP-IDF project root
  sdkconfig.defaults      # target defaults (ESP32-S3)
  components/
    jalak_logic/          # PORTABLE logic: no IDF headers, host-testable
      include/jalak/*.h   # expression, framebuffer, state_machine, audio, api_client
      src/*.cpp
    jalak_hal/            # hardware abstraction (interfaces + mocks only)
      include/jalak/hal/*.h   # display/audio interfaces, MockDisplay/MockAudio, MockTransport
    jalak_app/            # IDF glue (not built on host)
  test/                   # host tests (CMake + CTest), no ESP32 required
```

## Status vocabulary

- **implemented**: source written.
- **compiled**: built by a real toolchain; exit code recorded in `../.planning/EVIDENCE/OFFLINE-EVIDENCE.md`.
- **simulated**: exercised through mocks/fakes, not hardware or real provider.
- **hardware-tested**: run on a physical ESP32-S3. *Not claimed yet.*
- **unverified**: no evidence.

## Important

- `jalak_logic` deliberately depends on the C++ standard library only, so it can be
  compiled and unit-tested on the host. That is how this machine validates it now,
  because ESP-IDF is not installed here.
- Real driver wiring (I2S, I2C, GPIO, SH1106 vs SSD1306) is **unverified** until
  hardware is available and G1 evidence exists.
- The device API client (`jalak/api_client.h`) is transport-agnostic: it holds the
  turn/status/audio policy (caps, retries, response validation) and performs no I/O
  itself. The real ESP-IDF `esp_http_client` transport is **unverified**; host tests
  drive it with `jalak::mock::MockTransport`. No hostname/schema/model is invented.

## Host tests

```
export TMPDIR="$PWD/.buildtmp"          # /tmp is quota-exceeded on this machine
cmake -S firmware/test -B "$TMPDIR/fw_build" -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build "$TMPDIR/fw_build"
ctest --test-dir "$TMPDIR/fw_build" --output-on-failure
```

Targets: `jalak_logic` (core logic), `jalak_hal` (HAL mocks + mood render),
`jalak_api` (device API client vs mock transport).
