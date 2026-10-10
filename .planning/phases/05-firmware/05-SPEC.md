# Phase 05: ESP32 Voice and Expression — Specification

Created: 2026-10-09. Status: planning only; no firmware, tests, UI assets or device evidence produced.
Dependencies: Phase 1 accepted inventory/contracts/safety boundaries and Phase 4 accepted gateway/guardrails before adult E2E work.
Requirements: FW-01, FW-02, FW-03, FW-04, FW-05.

## Goal
A user initiates a bounded turn on the robot, understands its state, hears the response and can stop it.
Adult-lab proves robot mic→own VPS→STT/chat/TTS→robot speaker without an operational companion; child-local proves a separate non-AI local invitation.

## Current → target → acceptance
| ID | Current | Target | Acceptance after authorization |
|----|---------|--------|--------------------------------|
| FW-01 | ESP32-S3 reported owned; profiles not built | Distinct signed/deployment adult-lab and child-local binaries with separate permission | Child image lacks capture/cloud path and adult credential; control/token/mode-flip and backend-denial checks pass |
| FW-02 | INMP441/MAX98357A reported owned; alignment/clocks unverified | Verified standard-I2S RX/TX conversion, bounded DMA/RAM/PSRAM, half-duplex | Canonical silence/ramp/clipping fixtures and real mic/speaker evidence match formats; mic never active during playback |
| FW-03 | OLED/buttons/controller/GPIO unverified | Explicit PTT/start and STOP, monochrome BOOT/READY/LISTENING/UPLOADING/PROCESSING/SPEAKING/STOPPED/ERROR/BOOK_HANDOFF | Actual control/OLED bench path exposes activity; STOP works in every active state; capture inactive idle/stopped/child |
| FW-04 | No network client or cancellation evidence | Verified HTTPS session/turn/generation client, bounded transfers/polling/reset | Real approved adult device tracer works; wrong/stale/cancelled response, malformed audio and TLS/auth failure cannot play |
| FW-05 | Catalog/content/SD capacity unverified | Offline human-authored/recorded/licensed hook and bounded choices, quiet handoff | Provisioned child build starts/stops offline, uses available verified books or labelled DEMO bench content, no capture/network AI/child-data egress |

## Boundaries
- In scope: ESP-IDF 6.1.x device E2E first, then faults/resource bounds, followed by separate child-local E2E and provisioning/wipe validation.
- Exactly 05-01 and 05-02; phase dependencies are external preconditions, plan-local waves are 1 then 2.
- Phase 2 actual verified 6–12-title catalog required for field-labelled content; adult/local bench DEMO catalog does not require Phase 2 and must remain marked.
- Out of scope: assembled enclosure/thermal/60-minute readiness (06), field participants/consent/pilot (02/07), CAD/printing/wiring layout (03/06), adult provider implementation (04).
- No current firmware source, binaries, UI graphics, recordings, SD files, pin map, model or test output may be written by this planning task.

## Constraints
- ESP32-S3 N16R8 and ESP-IDF 6.1.x selected; exact patch/toolchain and actual hardware driver contract must be approved before future build.
- Two I2S controllers are documented for S3, not proof of actual mic slots/word alignment/controller/clock support. INMP441 raw24/32-bit slots must become PCM16, not be re-labelled.
- OLED 128×64/1.3-inch owner report does not identify SH1106 vs SSD1306; Phase 1 verifies controller/address/envelope, controls, GPIO and safe supply. No pins/layout invented.
- Input WAV PCM16 little-endian/16k mono ≤8 s =256,000 sample bytes +44 header; output WAV24k mono ≤15 s =720,000 sample bytes +44 header; hard cap768 KiB.
- Hold PTT to capture; release/8 s ends; STOP separately invalidates/cancels and clears output; no always-listening/full-duplex or microphone during playback.
- ≤4 turns/180 s, reply ≤35 words; 500 ms polls, original turn deadline30 s; unique device auth, TLS verification and backend ≤5 new turns/minute.
- DMA-capable buffers use suitable verified RAM; large buffers use measured PSRAM capacity with reserve. Nominal8 MB is not all free; checked allocations and bounded rings mandatory.
- OLED target10–15 FPS must not starve audio/network; state distinguishable without color or long paragraph text. Target, not measured performance.
- Child image is not an adult image with a mode flag: no microphone path, provider/network-AI permission, adult credentials or child-data egress; revoke/remove adult credentials before child deployment.
- Local SD capacity/free space/format/media unknown; provision only validated bounded human content, record provenance/license/hash, reject corrupt/path-traversing content.
- External regulated power only after approved inventory/bench checks; LiPo protection/runtime/charging not assumed safe. Hardware Rp500k and operating Rp100k are separate, neither spend approval.

## Acceptance criteria
- [ ] Each plan has its own new approval before any executor/workflow begins; build/flash/hardware/access/install/billing/Git/printing/field permissions remain separate.
- [ ] 05-01 first work task proves real microphone through the accepted backend to actual speaker and visible states; host/recorded/mock audio alone cannot pass.
- [ ] STOP/reset, no-mic playback, WAV/deadline/heap bounds and stale/cross-session denial are evidenced on approved bench hardware.
- [ ] 05-02 first work task proves human local content through controls/OLED/speaker with capture and networking excluded.
- [ ] Child credential wipe/revocation, signed profile verification and inspected egress/microphone tests pass before any child deployment; this does not approve a pilot.

## Edge coverage / prohibitions
| Edge or must-NOT | Required future check |
|------------------|-----------------------|
| Unknown pins/controller/clocks/power/SD capacity | Halt relevant build/flash/provisioning; consume actual Phase 1 evidence, never example wiring |
| Allocation/PSRAM shortage, underrun, malformed WAV/chunk/length, disconnect/invalid certificate | Bounded ERROR/STOPPED with buffer purge; no verify-TLS bypass or private-audio retry queue |
| STOP during capture/upload/poll/download/play; release/reset races and old generation | Local capture/playback stops immediately; stale result cannot play after reset |
| Mode/token spoof, reused adult build, resident adult credential, child capture/network AI | Compile/deployment/permission and wipe/revocation checks deny all; no proxy/guardian/paid-tier Gemini bypass |
| Missing/corrupt/out-of-stock local hook/book, media removal/full storage | Disclosed local/error/ask-human or quiet stop; no invented holdings, no secret cloud fallback |
| Always-listening, cloned voice, identity/emotion profiles, points/RAG/web tools | Excluded; no field data, firmware/UI code/assets or generated child scripts produced now |

## Assumptions and future facts
U: owned modules and available VPS are reports. D: two-I2S/SDK evidence and recorded provider terms. A: formats/profiles/state targets are planning selections.
V: actual hardware/codec/pins/models/latency/safety/free RAM/SD/readability/content and provider eligibility remain unverified; no ambiguity score or passing result invented.
