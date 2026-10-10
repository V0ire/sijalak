# Phase 04 — Recorded Research Supplement

Created: 2026-10-09. Planning only; synthesis of existing local sources, not new research or implementation evidence.
No queries, scripts, tests, builds, SDK/model calls, account/VPS access, installs, Git, hooks or global changes performed.

## Evidence levels
U = owner report; D-public = previously recorded public documentation; D-indexed = recorded Context7 official excerpts; A = selected planning contract; V = actual verification pending.
Documentation evidence establishes a source claim, not availability, legality or performance of the owner's particular account/client.
| Topic | Evidence/source | What remains unverified |
|-------|-----------------|-------------------------|
| VPS/API and STT/chat/TTS available; “Gemini, presumably” | U; `.planning/research/AI-RESEARCH.md` owner reports | Actual offering, host, schema, auth, account, codecs, models, quotas and billing |
| Developer API users18+; no client directed toward/likely accessed by under18s | D-public; recorded Google terms dated2026-04-28, read2026-10-09; https://ai.google.dev/gemini-api/terms | Eligibility of the separate restricted internal adult R&D client/account must be reviewed before calls |
| STT candidate `gemini-3.5-transcribe` | D-indexed; official changelog/transcription excerpts recorded in root research | Exact API/model ID, account access, price and Indonesian conformance |
| Chat candidate `gemini-3.1-flash-lite` | D-indexed; official model/audio excerpts recorded in root research | Supported dialog contract, actual availability, quality and cost |
| TTS candidate `gemini-3.8-flash-lite-tts`, prebuilt `Kore` | D-indexed; official changelog/speech excerpts; public speech fetch previously timed out | Actual voice/locale/codec/rates; old preview prices cannot price this candidate |
| FastAPI async/network handling | D-public; previously recorded https://fastapi.tiangolo.com/async/ | Actual Python3.12/dependency pins, compatibility, workload and deployment |
| ESP-IDF stable S3 page labelled6.1, two I2S controllers/DMA | D-public; recorded stable I2S documentation in root research | Actual device slots/clocks/GPIO/OLED driver and safe hardware; Phase05 checks device conformance |
| Isolated gateway, bounded jobs and budgets | A; existing04 SPEC/CONTEXT/PLANs | No gateway, installed dependency, tested fixture, rate snapshot or approved spend established here |

## Preserved contracts, not observed results
- D3 remains robot→own isolated VPS/API→STT/chat/validator/TTS→robot speaker, without an operational companion.
- Direct VPS/proxy/adult relay, guardian consent and a paid tier do not bypass the recorded child-client restriction; no child Gemini authoring pipeline either.
- Restricted adult internal R&D remains eligibility-gated; a different provider/offering needs separate reviewed evidence, not silent substitution.
- Python3.12/FastAPI async role adapters; namespace `/sijalak/v1`, host `UNKNOWN` is not an active URL; REST500ms polling/30s deadline, no default WebSocket.
- Canonical input WAV PCM16/16k mono ≤8s/256,044 bytes; output PCM16/24k mono ≤15s/720,044 bytes, hard768KiB.
- ≤4 turns/180s, ≤35 words, ≤5 new turns/device/minute; one global active job/no pending queue; schema/action/book allowlist before TTS, not prompt alone.
- Unique hashed device auth, TLS verification, ownership/idempotency/generation checks; provider secret VPS-only, never borrowed from OpenCode/Pro/9Router.
- Metadata-only logging; transientTTL10min/audio expiry60s; STOP/reset clears accessible state, not provider-side retention or billing.

## Future evidence and permission gates
- Phase01 actual inventory/contracts/readiness evidence precedes integration; owner reports and indexed candidates never substitute for approved actual schema/models/account/locale.
- New explicit approval naming04-01 or04-02 must precede any executor/workflow; planning YOLO and the other plan's approval cannot satisfy it.
- Server/access, dependency installs, provider/data, billing/spending and adult participation require separate authority; package legitimacy/compatibility/pins before dependency setup.
- Rp100k/month is an unpriced operating planning ceiling, not approval; Rp500k additional hardware is separate. Missing rates/FX/allowance denies calls.
- Synthetic/consenting-adult evaluation only: mocks/local fallback labelled and excluded from real-AI success; no child/private corpus or fabricated green results.
- Stage/backend latency can later be measured; actual robot-audible latency/safety needs05/06 evidence. Runtime feedback latency remains unknown.

## Source handoff
Sources: `.planning/research/AI-RESEARCH.md`, `.planning/AI-SPEC.md`, `.planning/VERIFICATION-STRATEGY.md`, `.planning/phases/04-vps-ai/04-SPEC.md`, `.planning/phases/04-vps-ai/04-CONTEXT.md`, `.planning/phases/04-vps-ai/04-01-PLAN.md`, `.planning/phases/04-vps-ai/04-02-PLAN.md`.
Requirement scope remains API-01…API-04 and AI-01…AI-06; no new feature, requirement, endpoint approval or existing-plan edit is introduced.
Future sampling/task/requirement/threat mapping lives in `.planning/phases/04-vps-ai/04-VALIDATION.md`; status stays draft and both runtime flags false pending separately authorized creation and verification.
