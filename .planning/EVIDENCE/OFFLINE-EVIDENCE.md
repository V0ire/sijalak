# OFFLINE-EVIDENCE — Software Workstream (AUTH-U-03)

Scope: offline-testable software for the ESP32-S3 firmware logic layer and the
credential-free backend core. **No hardware, no network, no provider credentials.**
This file records exactly what was built, the command lines used, and observed
results. It does **not** claim any hardware, field, or live-API verification.

Authorization: AUTH-U-03 (dependency review + offline software implementation).
Environment constraint: `/tmp` is quota-exceeded; **all builds use
`TMPDIR=<repo>/.buildtmp`**. No Git, no global config, no installs.

Status vocabulary (see `firmware/README.md`, `backend/README.md`):
`implemented` `compiled` `simulated` `hardware-tested` `unverified`.

---

## 1. Status summary

| Artifact | Status | Evidence |
|---|---|---|
| `firmware/components/jalak_logic` (expression, framebuffer, state machine, audio) | **implemented + compiled + simulated** | host build + ctest below |
| `firmware/components/jalak_hal` (interfaces + MockDisplay/MockAudio) | **implemented + compiled + simulated** | `test_jalak_hal` below |
| `firmware/components/jalak_logic` device API client (SW-FW-05) | **implemented + compiled + simulated** | `test_jalak_api` below (mock transport) |
| Device API client cancellation + deadline contract | **implemented + compiled + simulated** | `test_jalak_api` below (fake Clock, token) |
| Portable OLED codec (SSD1306/SH1106 command/page encoder) | **implemented + compiled + simulated** | `test_jalak_oled` below |
| IDF HAL drivers (I2C OLED, GPIO touch, I2S audio, esp_http transport) | **implemented**; IDF compile **unverified** | I2S is a real v5.4 `i2s_std` driver; all guarded by `ESP_PLATFORM` |
| `firmware/components/jalak_app` full composition | **implemented**; IDF compile **unverified** | full PTT→capture→upload→speak→ready flow on real FSM transitions |
| Backend core (`limits`, `sessions`, `reply_schema`, `guardrails`, `provider`, `cost`) | **implemented + compiled + simulated** | pytest below |
| Backend HTTP gateway (Starlette) + uvicorn entrypoint | **implemented + compiled + simulated**; real-socket E2E **verified locally** | `test_gateway_*.py` + manual uvicorn run below |
| Firmware→backend request contract | **verified offline (E2E)** | `backend/tests/test_firmware_e2e.py` builds the real C++ body and POSTs to the real gateway |
| Firmware↔backend wire contract | **simulated** | `test_contract_sync.py`, `test_wire_format.py`, `test_integration.cpp` |
| ESP32-S3 target firmware image | **unverified** (no ESP-IDF toolchain run) | installer fixed to fail-fast; toolchain still not installed |
| IDF component wiring (REQUIRES/SRCS/include) | **statically validated** | all component SRCS/include paths verified present; `esp_driver_*` REQUIRES corrected |
| I2C OLED / I2S mic+amp on real hardware | **unverified** | — |
| Live provider (Gemini) calls, capture, cloud path | **unverified** (no credentials; adult-R&D only) | — |
| Phase 1 goal / gates G1–G5 | **NOT_VERIFIED / BLOCKED** (unchanged) | `phases/01-baseline-contracts/01-01-SUMMARY.md` |

Global progress remains **0/37 requirements** — offline code does not satisfy any
requirement that requires hardware, field evidence, or live-provider behavior.

---

## 2. Firmware host build + tests

Toolchain: host g++ / cmake / ninja (ESP-IDF not installed).

```
cd <repo>
export TMPDIR="$PWD/.buildtmp"
cmake -S firmware/test -B "$TMPDIR/fw_build" -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build "$TMPDIR/fw_build"
ctest --test-dir "$TMPDIR/fw_build" --output-on-failure
```

Observed result (final run):

```
1/5 Test #1: jalak_logic ......................   Passed    0.00 sec
2/5 Test #2: jalak_hal ........................   Passed    0.01 sec
3/5 Test #3: jalak_api ........................   Passed    0.00 sec
4/5 Test #4: jalak_integration ................   Passed    0.00 sec
5/5 Test #5: jalak_oled .......................   Passed    0.00 sec

100% tests passed out of 5
```

Clean-rebuild receipt (`--clean-first`): **0 warnings**, then **5/5 passed** — reproduced
from scratch, not an incremental artifact.

- `jalak_logic` — 10 host tests: expression clamp, page-byte size, face render,
  state-machine paths, audio input/output caps, gesture handling.
- `jalak_hal` — HAL mock contract (display init/present/push_count, half-duplex
  audio) + all-moods render smoke test + state-expression coverage.
- `jalak_api` — **35** host tests (SW-FW-05): JSON scanning/decoding, response parse
  + rejection (bad action, overlong reply, non-2xx, advertised oversize), retry
  policy + backoff arithmetic, fail-closed pre-network size/idempotency checks,
  retry-then-succeed, no-retry-on-4xx, audio download caps, status-poll recording,
  plus the audit-fix regressions (D1 overflow, D2 book allowlist, L2/L3 JSON
  hardening, cancellation + deadline semantics). Driven entirely by
  `jalak::mock::MockTransport` / a fake `Clock` — **no socket, no TLS, no network**.
- `jalak_integration` — 5 cross-boundary tests: firmware parser consumes
  backend-generated wire JSON (done/handoff/status/error), including allowlist
  rejection parity.
- `jalak_oled` — 6 tests for the portable OLED command/page encoder (frame-size
  validation, init essentials, rotate-180 remap, full-frame transactions, SH1106
  column offset, bad-size rejection).

Sanitizer receipt (ASan + UBSan): all **five** host suites run clean via
`firmware/test/run_sanitizers.sh` (exit 0, no diagnostics).

### 2.0a Firmware completeness pass — I2S driver, HTTP contract, app flow

What changed from the earlier "composition" state (which had stubs on the main
path), each verified off-hardware by build + tests:

- **I2S driver is now real** (`jalak_hal/src/i2s_audio_device.cpp`): ESP-IDF v5.4
  `i2s_std` API (`i2s_new_channel` → `i2s_channel_init_std_mode` →
  `i2s_channel_enable`, `i2s_channel_write`/`read`), TWO channels with separate
  TX (amp, 24 kHz/16-bit) and RX (mic, 16 kHz/32-bit→PCM16) pins, half-duplex
  enforced. All macros/types checked against the local IDF headers.
- **Firmware builds the backend's exact request body**: portable
  `base64_encode` + `build_turn_request_body` produce
  `{"session_id","idempotency_key","wav_base64"}`; `parse_session_id` +
  `transport.create_session()` bootstrap a session. Earlier the POST sent only
  ids (no audio) and would have 4xx'd.
- **`app_main` runs the full flow** on real FSM transitions: PTT→Listening→
  (stop/timeout)→Uploading→submit→Processing→Speaking(play audio)→Ready; long
  press = STOP aborting the in-flight request. No stub remains on the main path.
- **IDF wiring corrected**: `PRIV_REQUIRES esp_driver_{i2c,i2s,gpio}` (the legacy
  umbrella `driver` would not expose those headers under v5.4), project-wide
  C++17, N16R8 `sdkconfig.defaults`. All component SRCS/include paths validated
  present.

Still **unverified**: no ESP-IDF toolchain run here → the target build and the
IDF compile of these drivers are not proven. No hardware.

### 2.0 Independent engineering audit + fixes (this session)

An independent audit reviewed the implementation (not the summaries) and found
defects, each fixed with a regression test that reproduces the failure first:

| ID | Severity | Defect | Fix | Regression test |
|----|----------|--------|-----|-----------------|
| D1 | High | `audio_bytes` > INT64_MAX overflowed `std::stoll`; the failure was swallowed and the size cap check was **skipped**, accepting an oversized turn | `scan_int(..., bool* malformed)` distinguishes absent vs malformed/overflow; parser fails closed | `test_d1_audio_bytes_*` (3) |
| D2 | Medium | Firmware accepted `handoff_book` with **missing or arbitrary** `book_id`; backend rejects both → ends diverged | added `parse_turn_response_with_allowlist`; handoff requires a curated id, any uncurated id rejected | `test_d2_*` (4) |
| D3 | Medium | `jalak_app` CMake declared a non-existent `INCLUDE_DIRS`, and `app_main.cpp` had no `app_main()` → target configure/link would fail | removed the bad include dir; added a real `extern "C" app_main()` using only portable symbols | `app_main.cpp` TU compiles clean |
| L2 | Low | `\uXXXX` unicode escapes were corrupted (`\u00e9` → `u00e9`) | full JSON string decoder incl. `\b\f\n\r\t`, `\uXXXX`, surrogate pairs, fail-closed on bad escapes | `test_l2_*` (4) |
| L3 | Low | Duplicate JSON keys resolved **first**-wins (JSON says last-wins), letting a hostile early value shadow the real one | `scan_string` now last-occurrence-wins | `test_l3_*` (2) |
| T1 | Medium | Timeout/cancellation had no contract; a timed-out, possibly non-idempotent POST could be blindly retried | added `CancellationToken` + `Clock` + deadline; aborts never retry; timeouts are returned as **ambiguous** (no retry) for caller-side reconciliation via `poll_status` | `test_cancel_*`/`test_deadline_*`/`test_timeout_*` (8) |

Cancellation/deadline semantics implemented and tested:

- Cancellation **before** a request → no network call at all (`posted.empty()`).
- Cancellation **between attempts** → retry loop stops.
- Explicit abort (`HttpResult.aborted` or token) → **never** retried.
- Deadline enforced with an inclusive boundary (`now >= deadline` stops); a
  boundary-not-yet-reached request proceeds.
- Timed-out `submit_turn` → classified `Kind::Timeout`, `ambiguous()==true`, **not**
  retried; caller must reconcile by idempotency key.
- Same cancellation/deadline guards applied to `poll_status` and `fetch_audio`.

Note on scope: the client-side cancellation contract is proven for the *loop*
(prevents sending / prevents further retries). It does **not** interrupt an
in-flight blocking transport read; that requires support inside the real
`HttpTransport` implementation, which is **unverified**.

### 2.1 Defect found and fixed this session

`draw_face()` reported `painted` incorrectly for wink/closed eyes. The wink
branch incremented `painted` while writing pixels, and the function then **rescanned
the whole 128x64 frame and added every set pixel again** — double counting.
Result: `Mischievous` (which winks) returned `painted=967` while the true count
was `934`.

Fix (`firmware/components/jalak_logic/src/framebuffer.cpp`): the paint lambdas are
now **paint-only**; a single authoritative post-pass scan is the sole source of
`painted`. `draw_mouth()`'s count-on-write contract was made explicit in a comment
(it must not rescan), and its `Surprised` branch now counts newly-set pixels
consistently instead of hard-coding `1`.

Verification — full mood matrix, `draw_face` tally vs. independent buffer scan:

```
export TMPDIR="$PWD/.buildtmp"
g++ -std=c++17 -I firmware/components/jalak_logic/include \
    "$TMPDIR/dbg.cpp" firmware/components/jalak_logic/src/*.cpp -o "$TMPDIR/dbg"
"$TMPDIR/dbg"
```

```
Neutral      painted=1802 count=1802 OK
Happy        painted=1186 count=1186 OK
Curious      painted=1598 count=1598 OK
Mischievous  painted= 934 count= 934 OK
Sleepy       painted= 486 count= 486 OK
Surprised    painted=1802 count=1802 OK
Annoyed      painted= 986 count= 986 OK
Thinking     painted=1486 count=1486 OK
Listening    painted=1802 count=1802 OK
Speaking     painted=1486 count=1486 OK
Error        painted= 902 count= 902 OK
```

All 11/11 moods agree. Exit code 0.

---

## 3. Backend tests

Toolchain: python3 + pytest (no network, stub provider only).

```
cd <repo>/backend
export TMPDIR="$HOME/Projects/sijalak2/.buildtmp"
rtk pytest -q
```

Observed result:

```
Pytest: 48 passed
```

Covered: canonical byte limits (input 256044 / output 720044), session ownership /
idempotency / TTL / generation, reply-schema action + book allowlists, guardrail
injection/harm markers, stub provider, fail-closed cost admission, firmware↔backend
contract sync, plus the new HTTP gateway suites:

- `test_gateway_http.py` (11) — full turn happy path, auth, unknown session,
  oversize→413, bad base64, idempotent replay, wrong-device isolation, reset
  invalidation, injection→safe_refusal, turn cap→429, handoff allowlist→422.
- `test_gateway_negative.py` (8) — provider failure→502 (+ observable `failed`
  status), reset-then-submit rejection, malformed JSON, missing fields,
  unknown-turn 404, audio-before-done 404, bad action→422, empty/undersized WAV.
- `test_firmware_e2e.py` (2) — compiles the REAL firmware `api_client.cpp` body
  builder into a stdin→body helper, POSTs its output to the REAL Starlette gateway
  over ASGI, and asserts 200 + the exact response fields the firmware parser reads.
  This closes the firmware↔backend request contract offline.
- `test_wire_format.py` (6) — locks the JSON wire shape the firmware hardcodes.
- `test_entrypoint.py` (2) — `python -m jalak_backend` arg/DI wiring.
- `test_contract_sync.py` (5) — firmware↔backend numeric/action parity.

### 3.0 Real-socket E2E (manual, verified)

Started the gateway under uvicorn (`python -m jalak_backend --port 8791`) and drove
it over a real HTTP socket:

```
POST /v1/session -> 200 {"session_id":"..."}
POST /v1/turn    -> 200 {"schema_version":"1","turn_id":"...","status":"done",
                         "reply_text":"Ini respons contoh dari mode uji.",
                         "action":"continue","audio_bytes":96044}
```

The response is exactly the wire JSON the firmware parser consumes (also asserted
by `test_jalak_integration`). Stub provider only — no network, no credentials.

---

## 3.1 Framebuffer memory assessment (no refactor)

Measured (host, `sizeof`/heap): a 128x64 `Framebuffer` holds `bits_` as
`std::vector<std::uint8_t>` at **1 byte per pixel = 8192 bytes** persistent, versus
a bit-packed minimum of 1024 bytes (8x overhead, 7168 bytes more). Each
`to_page_bytes()` call additionally allocates a transient **1024-byte** buffer.

Risk: LOW for v0.1 — ESP32-S3 has ~512 KB SRAM, so ~8 KB is affordable, and only
one framebuffer exists at a time. Recorded as a known inefficiency; **no refactor
performed** (unnecessary for current scope). If the panel or buffers multiply
later, switch `bits_` to a packed representation.

---

## 4. Artifact digests (sha256)

Firmware logic (`firmware/components/jalak_logic/`):

```
19a34c8172c134c6195ab49d0b2e7fbe45ed019d213c9cb6713a7900cf128b34  src/audio.cpp
f20fd7ed91eed5396416725ac998aebf99e055b9e462f64670f867a052846430  src/expression.cpp
e1f5f89ea1023980bf3dbcf25b3e75bcc993484a9cdaf98e6bef002c9db7ca3a  src/framebuffer.cpp
75db2f9f1563bf33b1badc4826d3cb7d6c5b77cfa6311160ca3abb26c0ab3b86  src/state_machine.cpp
c07d9db7805a053568459dbc4be19ea89033419b435fca70d62ade0212c9139a  include/jalak/audio.h
f71da935ce36477768898d2ad6e8077e735bd80b18b8b54554a80d640b90bbd2  include/jalak/expression.h
ae838ddda4188aa8188bd040cb409dbf894bfe98cfdb5c0e3ebd7bfed1e398f0  include/jalak/framebuffer.h
0e6af620f168a8254606d362eb0f2b66eb0798d06a91c646c818c45722b08b87  include/jalak/logic.h
931d4ee2762f2e99a8491ea0384efe31d4899b69d7dc9f7aca2a17615ecfef59  include/jalak/state_machine.h
04c939d694ec2f659c6583e119a90b9cb1f7882a43f245bac4f6334ac3bfac0e  src/api_client.cpp  [superseded]
e4092410b58ad35aa89e56e2b9ceeae94d9e311ad74e337d6b6318d5e6da820b  include/jalak/api_client.h  [superseded]
1197e7dc009b571b3956046cc4c2b0f4904c58841f5b53724bcd983932b1a8ab  src/api_client.cpp
5efa60997a102ad3826db819545bd57aec1c01d3872630753b0ea481a082aa6a  include/jalak/api_client.h
```

HAL mock + app glue + host tests (audit-fix session):

```
f7e22afe3535a4c4fdbbb7a5d831c38f21e20267862d9d1e19541598ea37eccd  include/jalak/hal/mock_transport.h
2b747d846a40a2bf3fc3dc551ba5fc9b5ad96322a12b3ba045e650ad1b356c9d  firmware/components/jalak_app/src/app_main.cpp
b52786c706de9181aec8bf969958830e09908e25255fc6dadb0186c67a9037c0  firmware/components/jalak_app/CMakeLists.txt
4ff163b657469379353d6a68c320909fdbd39eed0b1495a7314144e73c142d7f  firmware/test/test_api.cpp
```

HAL mock (SW-FW-05 harness):

```
023570d00869b51177d24fdb4ad9f7d6116ad03655b5901c04cb53e24008b6c7  include/jalak/hal/mock_transport.h
```

Host test sources (SW-FW-05):

```
23330cdab183f29514953727957965299f1d2a14a4c334409454b0ead8b9b60d  firmware/test/test_api.cpp
```

Backend (`backend/`):

```
10a3fe776d3fb6f9c726a270d7159f3b377372263fb3e2c7b7f5c6363c4dcea8  jalak_backend/cost.py
2aeca2ccff12103393e0a28f76239bf236b74881d4f37b6d063a17d868622de3  jalak_backend/guardrails.py
055c1b5567128e873cb3e8a718ca428118ffa14c48fbd7cdd040b953cb2ebb78  jalak_backend/__init__.py
e528e7556719f163dda5effa564cc82ff6029fa6c69f09e0bd9a045206f40812  jalak_backend/limits.py
15353e7f836b87ab3e12c6a7a34b65d149d5c0918ff2b16758659659d6878fc4  jalak_backend/provider.py
8bc5c4f4fafd930f356691a62787fd2435d313e2d70fc58db178ac56fc7a48e0  jalak_backend/reply_schema.py
9bee36875e46c592e2142ca7ed5cde2c58c42afbadcdf957cedbd18515005dbf  jalak_backend/sessions.py
5252db05a46c32e673ae7c3f450f320a02ee807fdc47e2d1ac431b33fb6594f4  tests/conftest.py
e77e6da4bb4e21a68993e3310205075c6dbe50c968e193ef08a99401f2a4594f  tests/test_core.py
4ded67ec02b2da6389ee89e20dd774bb364db3d6d78730cf1cbe8ba953e7ad5f  tests/test_contract_sync.py
```

Backend HTTP gateway + new firmware artifacts (gateway milestone):

```
e924413178bbf4604b2eb2d8b837547304dfb306971ca54ac5d29ef0af7231e8  backend/jalak_backend/gateway.py
2463b9784454335a94b302513d9a9931c143abfa70b6e2119f752cb6118e7634  backend/jalak_backend/wire.py
6032e045c39cb88d665de7b61c3556f14e362d1e17bd354ea8dbb0905cb71498  backend/jalak_backend/__main__.py
96b71eef11ca9f6045d820f849623f4fad97e45758e7439a623e5ae510992ea2  firmware/components/jalak_logic/src/oled_codec.cpp
1fbb13a295f29987fca3e714b7954429beb7511fda4a190f1dc7f70db1fcf8c0  firmware/components/jalak_logic/include/jalak/oled_codec.h
9340003dfd1465c17b26294670c04dd9f96b4411b9d69ff6f51c7bb5b0b1712e  firmware/components/jalak_hal/src/i2c_oled_display.cpp
1d3055d4f75662c6b1a8510672b7ffd9d4d82a7c8a560e18942f458077e7e5f4  firmware/components/jalak_hal/src/touch_input.cpp
42dd4b9b32af2819dd204e686d61e1d0be2624b35fe98402653ae00bf4038fcf  firmware/components/jalak_hal/src/i2s_audio_device.cpp
541d6796b7879a69cd2cd2abf591c4ee1a482d243642804dc6b9653eaa6be467  firmware/components/jalak_hal/src/esp_http_transport.cpp
be9955e14059f82f31f87c27d3c9cbb0bd98149c4ba384effc5a08006efddcf2  firmware/components/jalak_app/src/app_main.cpp
```

---

## 5. What this evidence does NOT establish

- No ESP-IDF compile of the ESP32-S3 target image: the toolchain is **not installed**.
  The IDF HAL drivers (`i2c_oled_display`, `touch_input`, `i2s_audio_device`,
  `esp_http_transport`) and `app_main.cpp` exist and are guarded by `ESP_PLATFORM`,
  but only their host "compiles-to-nothing" behavior is verified here; real IDF
  configure/link/compile is **unverified**. Installer prepared: `tools/install-esp-idf.fish`
  (uses the uv-managed Python 3.12; needs the user to run it — no sudo available to the agent).
- No I2C OLED, I2S microphone, amplifier, touch controller, or power-path validation.
- No live provider call, no microphone capture, no cloud round-trip.
- No real HTTP/TLS transport: the device API client is proven only against a mock
  transport; the backend gateway is proven over a **local loopback** uvicorn socket
  with a stub provider. Real host/endpoint/schema/model/TLS remain unknowns.
- Cancellation/deadline are proven for the **retry loop** only; interrupting an
  in-flight blocking transport read requires real `HttpTransport` support and is
  **unverified**.
- No field/TBM session data; no child-facing behavior observed.
- Gate G1 (hardware readiness) remains **BLOCKED**; phase 1 goal remains **NOT_VERIFIED**.

Still requires separate authorization: hardware powering/testing, live API calls,
deployment, Git init/commit, purchasing, printing, global config changes.
