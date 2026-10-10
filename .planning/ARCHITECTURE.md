# Architecture: Si Jalak Bedas v0.1

Auto-selected planning baseline2026-10-09. Not deployed/tested. Sources, trade-offs and revisions: `DECISIONS.md`. Requirements: `REQUIREMENTS.md`. This document specifies contracts, not implementation code or electronic layout.

## 1. Boundaries and modes
1. **Adult-lab D3**: restricted internal18+ engineering client; robot → own VPS/API → Gemini role adapters → robot speaker. No operational laptop/phone companion. Eligibility/account/terms/access must be reviewed before live requests.
2. **Child-local pilot**: separate firmware/config/artifact with no mic capture, no Gemini/network AI permission, human-authored/recorded/licensed local book hooks and choices. Not an age checkbox in the same Gemini consumer client. Provider contract suitable for children is a future gate, not presumed.
3. **Development/admin**: a computer may prepare content/firmware or administer services after approval; it is not required during a visitor turn.

Existing VPS/API capabilities are owner reports. “Everything Gemini, presumably” does not identify actual transport/model/billing account. Never modify or borrow secrets from global OpenCode/Pro/9Router/plugins automatically. Proposed service namespace is isolated; existing APIs may remain upstream after an authorized contract check.

## 2. Responsibilities
| Layer | Selected responsibility | Not selected / prohibited by default |
|-------|-------------------------|---------------------------------------|
| ESP32-S3 N16R8 | Control/OLED state, bounded half-duplex I2S audio, explicit input, HTTPS client, cancel/reset, local-content profile | Local general LLM, camera/presence/always-listening, locomotion, identity profiles |
| VPS gateway | Async request jobs, device/session ownership, audio normalization, STT/chat/TTS adapters, response policy, quotas, metadata | Changes to unrelated services, autonomous web tools, arbitrary model commands, permanent voice logs |
| AI providers | Transcription, bounded dialogue, speech synthesis for eligible adult lab | Child-directed Gemini access, undocumented retention promises, unverified model availability |
| Operator | Configure content/profile, authorize sessions/participants, stop, maintain power/storage | Reading diagnosis, hidden observation or pretending fallback is live AI |
| Physical enclosure | Character, display/audio openings, payload envelope, repeatable service access | Detailed PCB mounting/coordinates/wiring or battery-power safety inferred from shape |

## 3. Runtime stack decision
- Firmware: **ESP-IDF6.1.x**, FreeRTOS tasks; patch/toolchain pinned after authorization, not installed now. Official stable S3 I2S documentation currently labels v6.1 and two controllers with separate RX/TX. Prefer standard-I2S roles; exact raw24/32-bit alignment and GPIO remain inventory-dependent.
- Backend: **Python3.12 + FastAPI**, async SDK/network calls; blocking codec/CPU work off event loop. Google GenAI SDK behind adapters, exact package versions pinned after contract audit. No heavy local model/GPU assumption and no queue/database infrastructure added for one-device demo unless measurements require it.
- Persistence: per-device auth/policy metadata and small curated catalog; transient job/session state with expiration. Proposed SQLite metadata store for one-worker demo, not voice/transcript history. Cross-worker deployment requires verified shared-store/ownership design first.
- Provisioning: deployment/HTTPS certificate/secrets/credentials/install steps require separate authorization; none executed in planning.

## 4. Protocol baseline — proposed, not existing API documentation
| Operation | Purpose / bounded result |
|-----------|--------------------------|
| `POST /sijalak/v1/sessions` | Authenticated adult-lab device starts a session; opaque session_id, expiry and enforced limits returned |
| `POST /sessions/{session_id}/turns` | Binary canonical WAV with idempotency key, turn_id/generation and metadata. Return202 job ID/status; never send upstream key to device |
| `GET /sessions/{session_id}/turns/{turn_id}` | Device-owned status and validated reply metadata; bounded polling every500ms, total job deadline30s |
| `GET /.../turns/{turn_id}/audio` | Device-owned normalized audio, content length/type/checksum; no unbounded provider URL/proxy access |
| `DELETE /sessions/{session_id}` | Invalidate session/generation, stop output, clear transient state; cancellation cannot promise provider-side deletion |

All routes are proposed under the namespace; shortened paths above inherit `/sijalak/v1`. Resolve actual existing gateway contract in phase1. Polling is selected for debuggable sequential turns; WebSocket/streaming deferred until measured latency justifies it.

### Audio and response contracts
- Input: canonical WAV PCM signed16-bit little-endian mono16kHz; max8s → **256,000 bytes samples +44-byte canonical header**. Accept only a declared validated format; oversized/malformed/silent input gets bounded failure. Raw INMP441 standard-I2S24-bit data must be normalized, not re-labelled.
- Output: normalized WAV PCM16 mono24kHz, max15s → **720,000 bytes samples +header**; hard download cap768KiB. Provider base64/codec conversion occurs on VPS, not assumed compatible on robot.
- Reply schema concept: `reply_text` <=35 words, `action` in continue/handoff_book/safe_refusal/end_session, `book_id` nullable and in current allowlist, schema/version/turn_id/generation. No GPIO/firmware/tool instructions accepted from model.
- Limit adult session to4 turns/180s. Controller deadline/stop dominates provider completion; wrong/stale/cancelled generation never plays. Pilot local invitation aims to leave attention on the book within90s, not a validated field outcome.

## 5. Device state and concurrency
States: BOOT → READY → LISTENING → UPLOADING → PROCESSING → SPEAKING → READY/BOOK_HANDOFF; STOPPED/ERROR from any active state.
- PTT hold activates capture; release or8s cap ends capture. Separate STOP cancels session/output immediately. No mic capture during playback and no unsolicited continuous invitation.
- Allocate DMA buffers appropriately for target peripheral; large bounded utterance/response buffers may use verified PSRAM. Start at small ring buffers, verify heap fragmentation and actual free memory. Nominal8MB PSRAM is not all available application heap.
- UI task at proposed10–15fps cannot starve audio/network. Distinct eye posture/motion signals state without relying on color or long128×64 text. No animation/UI code or image assets generated now.
- Child-local profile captures no audio; PTT-labelled general start control instead selects/plays curated content as configured. STOP still works. User controls cannot switch it to adult/cloud mode.

## 6. Auth, safety and operations
- TLS server verification; high-entropy per-device limited-scope credential, server-side hashed lookup, rotation/revocation. No admin/provider credential in firmware, URL or logs. Exact secure storage supported by actual board must be checked.
- Backend enforces device class and session ownership, mode, rate/payload limits, expiry and idempotency. Duplicate submissions must not cause duplicate paid inference; provider retry only with known safe policy.
- Per-device adult-lab allowance5 **new turn submissions**/min plus4/session; polling/status is separately capped at3requests/s with small burst, so500ms polling is not accidentally throttled by the paid-turn limit. Budget admission before provider invocation; unconfigured pricing/billing blocks live calls.
- No raw audio/transcript/reply logs by default; metadata only (duration/status/stage time/cost counters). Session TTL10min, audio result expiry60s after completion; reset/stop purges accessible local/backend transient data. Google may retain paid-service content for security; do not claim remote zero retention.
- Proposed operating ceiling Rp100k/month is a planning placeholder, not permission; model prices and FX must be snapshotted. Hardware budget allocation remains250kcase/100kcontrols-power-fasteners/150kreserve, quotes pending.
- Safety response validation + operator stop + dataset tests, not prompt alone. No web browsing/tool agent, emotion identification, voice cloning or long-term visitor memory.

## 7. Power and hardware constraints
- First adult demo uses a suitably verified regulated external5V supply; planning candidate at least2A. It is not permission to inject5V into3.3V logic or combine unverified supplies.
- LiPo450mAh at3.7V stores nominal~1.665Wh. Runtime depends on actual Wi-Fi/audio/regulators; no runtime guarantee. Disable charging/load-sharing/battery field operation until module protection/power path and thermal checks are evidenced.
- Inventory controls/buttons, display controller, actual speaker and battery envelopes in phase1. No GPIO map, wire routing, PCB mounting or detailed internal construction is frozen in this planning.
- Envelope/interface constraints from `INDUSTRIAL-DESIGN.md` are inputs, not proof of simultaneous component fit or cooling. Printing cannot certify electrical/child safety.

## 8. Failure behavior / dependencies
| Failure | Required response |
|---------|-------------------|
| No network / TLS/auth failure | Do not capture/queue private audio indefinitely; visible local error, operator stop, no bypass of certificate checks |
| STT empty/unsupported speech | Ask bounded repeat or end; no hallucinated transcript or emotion diagnosis |
| Invalid/unsafe dialogue | Safe short refusal or end; no model-generated arbitrary action |
| TTS fails / audio format wrong | Disclosed local fallback or end; not counted as real-AI integration success |
| Reset/duplicate/stale result | Session/generation checks prevent replay into next visitor; clear microphone/buffers/context |
| Budget exhausted / unknown rates | Admission denied; no unlimited retries or billing auto-enable |
| Power/heat/mechanical concern | Stop before participant use; operator intervention; no child debugging |

Dependencies: phase1contract/inventory → phase4gateway → phase5device → phase6demo. Industrial phase3 runs parallel and joins6; field phase2 joins7 with verified local profile/provider eligibility. The entire current work stops at planning; see `STATE.md` and per-plan first checkpoints.

## Sources
ESP-IDF stable S3 I2S: https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/i2s.html (live read2026-10-09, titlev6.1). FastAPI async: https://fastapi.tiangolo.com/async/ (live read same date). Gemini/model/terms evidence: `AI-SPEC.md` and `research/AI-RESEARCH.md`.
