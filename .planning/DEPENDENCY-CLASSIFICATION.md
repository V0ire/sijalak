# Dependency Classification — 33 tasks

Date: 2026-10-09. Owner authorized a transparent dependency review and re-scoping so that **software that does not depend on physical evidence may start now**. This document classifies all 33 planned tasks. It does **not** reopen Phase 1 documentation, delete acceptance criteria, or mark any gate verified.

## Authorization and limits

- **Granted (AUTH-U-03):** revise scope/dependencies, and implement offline-testable software for the ESP32-S3 firmware foundation and backend components that need no hardware, no credentials, and no external provider.
- **Still separate authorization:** hardware verification/powering, live API/provider calls, deployment, Git initialization/commits, purchasing, printing, and global configuration changes.
- **Preserved:** G1 hardware readiness stays **BLOCKED**; no gate is faked; no hardware is claimed verified.

## Classification legend

| Bucket | Meaning |
|--------|---------|
| **OFFLINE** | Implementable and host-testable now, no hardware/network/field input |
| **HW** | Requires physical hardware or measurement |
| **API** | Requires external provider/account/credentials |
| **FIELD** | Requires site access, participants, or owner decisions |

## Per-task classification

| Plan | Task | Type | Bucket (primary) | Notes |
|------|------|------|------------------|-------|
| 01-01 | 1–3 | docs | DONE | Documentary packet complete; goal unverified |
| 02-01 | 1–3 | field | FIELD | TBM partner/consent/observation/catalog |
| 03-01 | 1–3 | CAD | HW | Enclosure geometry/fit/service |
| 03-02 | 1–3 | CAD | HW | Slicing/print/fit measurement |
| 04-01 | 2–3 | backend | **OFFLINE + API** | Adapter/auth/limits/idempotency testable offline behind stubs; a real turn needs API |
| 04-02 | 2–3 | backend | **OFFLINE (guardrails/eval harness)** | Prompt-injection/oversize/stale/dedup/cost logic testable with a stub provider |
| 05-01 | 2–3 | firmware | **OFFLINE (logic) + HW (I/O)** | State machine, eye states, mock audio bus, API client contract testable offline; real mic/amp/OLED needs HW |
| 05-02 | 2–3 | firmware | **OFFLINE (logic) + HW** | Child-local mode routing, credential-wipe logic, SD content parsing testable offline; real wipe/SD needs HW |
| 06-01 | 1–3 | integration | HW + API | Enclosed-robot live turn |
| 07-01 | 1–3 | field | FIELD | Child-local pilot with participants |
| 08-01 | 1–3 | ops | FIELD | Real release/operator lifecycle |

## Extracted OFFLINE workstream (new, executable now)

These are the software slices the owner authorized. They live **before** the hardware/API gates and are tracked as a new exploration stream rather than faking phase completion.

| ID | Deliverable | Status target this session |
|----|-------------|----------------------------|
| SW-FW-01 | ESP-IDF project skeleton + CMake/components layout | implemented + host-buildable |
| SW-FW-02 | Portable eye renderer + OLED abstraction (controller-agnostic, SH1106/SSD1306 selectable) | implemented + unit-tested |
| SW-FW-03 | Interaction state machine + gesture logic | implemented + unit-tested |
| SW-FW-04 | Audio interface + mock/test harness (PCM16 framing, half-duplex rule) | implemented + unit-tested |
| SW-BE-01 | Backend adapters with stub provider (auth/limits/idempotency/session) | implemented + unit-tested |
| SW-BE-02 | Response guardrail/schema validator + eval harness | implemented + unit-tested |
| SW-FW-05 | Device API client contract logic (testable against a mock server) | implemented + unit-tested |

## Progress ledger (updated 2026-10-09, AUTH-U-03)

Evidence: `.planning/EVIDENCE/OFFLINE-EVIDENCE.md` (command lines + observed results + digests).

| ID | Status | Evidence summary |
|----|--------|------------------|
| SW-FW-01 | **implemented + compiled + simulated** | `firmware/` CMake/components tree; host cmake+ninja build exit 0 |
| SW-FW-02 | **implemented + compiled + simulated** | `jalak_logic` framebuffer + `jalak_hal` MockDisplay; 11/11 mood render matrix agrees |
| SW-FW-03 | **implemented + compiled + simulated** | `jalak_logic` state machine; host test paths pass |
| SW-FW-04 | **implemented + compiled + simulated** | `jalak_logic` audio limits + MockAudioSource/Sink; half-duplex contract test passes |
| SW-BE-01 | **implemented + compiled + simulated** | `backend/jalak_backend` sessions/limits/auth logic; pytest 12 passed |
| SW-BE-02 | **implemented + compiled + simulated** | guardrails + reply_schema; pytest 12 passed |
| SW-FW-05 | **implemented + compiled + simulated** | `jalak_logic` device API client + HAL `MockTransport`; 35 host tests pass (no network) incl. cancellation/deadline |

Host test totals this session: ctest **5/5 passed** (`jalak_api` = 35 tests,
`jalak_integration` = 5, `jalak_oled` = 6); backend **46 passed**.
Requirements completed: **0/37** (offline code satisfies no hardware/field/live-API
requirement). Gate G1 remains **BLOCKED**; phase 1 goal remains **NOT_VERIFIED**.

All seven SW items (SW-FW-01..05, SW-BE-01..02) are now `implemented + compiled +
simulated`. The remaining unverified pieces are hardware (board, OLED, mic, amp,
power) and the real HTTP/TLS transport — none of which are in the offline scope.

### Gateway / HTTP milestone (same session)

- Backend HTTP layer on **Starlette** (chosen because `fastapi` is not installed;
  Starlette/uvicorn/pydantic/httpx are). Routes `/v1/session`, `/v1/turn`,
  `/v1/turn/{id}`, `/v1/turn/{id}/audio`, `/v1/session/{id}/reset`; auth via
  `x-device-token`; idempotency, input/output caps, guardrail, curated book allowlist.
- `wire.py` locks the JSON contract; `test_wire_format.py` + `test_jalak_integration`
  assert firmware and backend agree.
- **Verified real-socket E2E**: uvicorn + `python -m jalak_backend`, session + turn
  round-trip returns the exact device wire JSON (stub provider).
- Portable OLED codec (`oled_codec.*`) + IDF HAL drivers written (`jalak_hal/src/`),
  guarded by `ESP_PLATFORM`; **IDF compile unverified** (toolchain not installed).

### Defects fixed this session

- `draw_face()` double-counted `painted` for wink/closed eyes (write-time increment
  plus a full-frame rescan). Fixed to paint-only + single authoritative scan;
  `draw_mouth()` count-on-write contract documented and `Surprised` count made
  consistent. Verified across all 11 moods.
- Added firmware↔backend contract-sync test (`test_contract_sync.py`) so the two
  ends cannot silently drift on caps/actions/word limit.

### Independent audit fixes (same session)

- **D1 (High)** — `audio_bytes` overflow skipped the size cap (fail-open). Fixed
  fail-closed in `scan_int`/parse; regression `test_d1_*`.
- **D2 (Medium)** — firmware accepted `handoff_book` with missing/arbitrary
  `book_id`, diverging from backend. Fixed with `parse_turn_response_with_allowlist`;
  regression `test_d2_*`.
- **D3 (Medium)** — `jalak_app` declared a non-existent include dir and lacked
  `app_main()`. Fixed CMake + added entry point; `app_main.cpp` host-TU compiles.
- **L2/L3 (Low)** — JSON unicode escapes corrupted and duplicate keys first-wins.
  Fixed decoder + last-wins; regression `test_l2_*`, `test_l3_*`.
- **T1 (Medium)** — no timeout/cancellation contract; a timed-out non-idempotent
  POST could be blindly retried. Added `CancellationToken`+`Clock`+deadline;
  aborts never retry, timeouts are ambiguous (no retry); `test_cancel_*`/`test_timeout_*`.

## Status vocabulary (used in evidence)

**implemented** — code written. **compiled** — built by a real toolchain with exit code recorded. **simulated** — exercised with mocks/stubs, not real hardware/provider. **hardware-tested** — run on physical ESP32-S3 + peripherals. **unverified** — no evidence yet.

None of the offline work may be reported as hardware-tested or as clearing G1–G5.
