# 01 Contracts — revision 2 DOC-FIRST packet

**Scope:** Tasks 2–3 documentary interfaces and pin dispositions. **G1: BLOCKED; readiness: BLOCKED; phase_goal: NOT_VERIFIED; integration_admission: false.** No endpoint, model, account or device was accessed/tested. Source IDs resolve in [01-VERIFICATION.md](01-VERIFICATION.md#source-register). U/D/A/V separation governs every matrix; proposed interfaces are not active API documentation.

## OLED device contract dependency

| record / stage / operation | reported_value | public_reference_value | proposed_value | actual_sourced_value | source_id/date/revision | evidence_class | discrepancy | required_gate |
|---|---|---|---|---|---|---|---|---|
| CON-OLED-01 / display interface and fit | Owned 1.3-inch 128×64 OLED | Representative Waveshare OLED (A): SH1106, board 40.5×37.5 mm, active 29.42×14.70 mm | Select driver and later aperture/service constraints only against actual owner part; isolated ESP-IDF 6.1.x candidate | Actual label/controller, bus, voltage, GPIO, driver/toolchain patch and envelope UNKNOWN | SRC-U04; SRC-OLED-C2; SRC-ARCH-A; 2026-10-09 local records | U / D / A / V | Public representative cannot identify owner hardware or establish electrical compatibility | G1 owner-specific hardware/pin evidence and separately approved work; no driver/fit freeze |
| CON-STT-01 / speech transcription | STT available; “Gemini, presumably” | Existing indexed official model references are candidates, not private provisioning | Role adapter behind isolated async FastAPI/Python 3.12; Bahasa Indonesia; canonical input below | Hostname, real routes/auth/schema, exact model/account, locale, rates, retention and processing location UNKNOWN | SRC-U08-U09; SRC-AI-D; SRC-ARCH-A; 2026-10-09 | U / D / A / V | Availability report does not prove normalization or deployment | G1 authentic sanitized contract/pins; G4 eligibility and separate service/call/billing permission |
| CON-CHAT-01 / bounded dialogue | Chat available; provider tentative | Recorded Gemini Developer terms restrict age/client audience | ≤35 words; ≤4 turns/180 s; adult internal R&D only if eligible | Provider offering/model/account/schema/price/retention UNKNOWN | SRC-U08-U09; SRC-TERMS-D; SRC-ARCH-A; 2026-10-09 | U / D / A / V | Public terms are not owner-account rights or deployed response behavior | G1 contract review and G4 eligible restricted adult client; unknown price denies live calls |
| CON-TTS-01 / synthesized output | TTS available; provider tentative | Public speech-generation excerpts were indexed; original live fetch timed out | Prebuilt voice, no cloning; Bahasa Indonesia; VPS normalization to output below | Real voice/model/format/rate/account/language support and processing location UNKNOWN | SRC-U08-U09; SRC-AI-D; SRC-ARCH-A; 2026-10-09 | U / D / A / V | Indexed examples cannot establish owned pipeline codec or locale conformance | G1 exact sourced pins and format proof; separate G4/call permission |

## Proposed policy, not actual API facts

SRC-ARCH-A (`../../ARCHITECTURE.md` §§3–7), SRC-POLICY-A (`../../DECISIONS.md` D-03–D-10), and current revision 2 govern these A proposals:

- Half-duplex PTT with separate STOP; no capture during playback. Input WAV PCM16 signed little-endian mono 16 kHz, ≤8 s, 256,044 canonical bytes. Actual 24/32-bit DMA/raw-slot alignment, conversion, clocks and pins UNKNOWN; raw mic data is not automatically canonical PCM16.
- Output WAV PCM16 mono 24 kHz, ≤15 s, 720,044 canonical bytes; download cap 768 KiB. Provider output normalization is proposed, not a demonstrated implementation.
- REST jobs under proposed `/sijalak/v1`; hostname UNKNOWN. Poll every 500 ms with total deadline 30 s; status cap 3 requests/s with small burst, separate from 5 NEW turn submissions/minute. Session ≤4 turns/180 s; reply ≤35 words; TTL 10 min and result expiry 60 s after completion.
- Exact package patch/toolchain/model/voice/schema/account/rates/FX pins UNKNOWN. FastAPI/Python 3.12 and ESP-IDF 6.1.x are selections, not installed or deployed versions. No silent substitutions.

## Eligibility and privacy disposition

SRC-TERMS-D is the recorded Google Gemini API Additional Terms snapshot: URL https://ai.google.dev/gemini-api/terms, prior live fetch 2026-10-09, page last updated 2026-04-28. It requires users 18+ and prohibits API clients directed toward or likely accessed by under-18s. It is not a finding about every offering named Gemini. Actual owner offering/account and adult internal-client eligibility remain V; no calls permitted here. Parent, adult proxy or paid tier is not an established exception; do not forward children's speech through adults.

Child-local proposal has no Gemini/cloud AI or microphone capture, including no Gemini generation pipeline for child scripts. It uses human-authored/recorded or licensed local material; actual content and technical isolation proof remain pending. Adult and child artifacts/permissions must remain separate, not an age-toggle workaround.

Metadata-only logging, transient expiry and local/backend purge on STOP/reset are proposed. No raw audio/transcripts, participant identifiers or credentials are recorded here. Cancellation cannot promise provider-side deletion or zero retention; SRC-TERMS-D records possible security/abuse retention even for paid service. Actual retention/deletion contract UNKNOWN.

## Cross-record admission

CON-OLED-01 retains SRC-OLED-C2 as D and INV-OLED-01 actual fields as UNKNOWN. [SAFE-OLED-01](01-SAFETY-BUDGET.md#oled-power-cost-and-fit-disposition) denies power/fit/spend admission; [verification](01-VERIFICATION.md#source-to-readiness-tracer) maps BAS-01/02/03 without completing them. [AUTH-U-02](01-AUTHORIZATION.md) permits these documents only; all separate operation grants remain NOT_GRANTED.

## Proposed route and response matrix

Source SRC-ARCH-A §§4/6, local v0.1 dated 2026-10-09; adopted by revision 2. Reported value for every operation: generic stage availability U (SRC-U08-U09), no specific route report. Public references do not establish private routes. Actual hostname, TLS endpoint, methods, schema/revision, status/content-type/checksum/auth contract for **every row are UNKNOWN (V)**. No endpoint is probed; G1 integration freeze BLOCKED until authentic sanitized service-custodian evidence and separate access scope.

| Operation ID | Proposed method/path (A) | Proposed request / response behavior (A) | Actual sourced value (V) | Discrepancy / required gate |
|---|---|---|---|---|
| ROUTE-SESSION | POST `/sijalak/v1/sessions` | Scoped authenticated adult device; opaque session_id, expiry and enforced limits; exact success status/serialization not frozen | UNKNOWN | Generic U availability cannot identify session API; source contract/schema/date/revision before integration |
| ROUTE-TURN | POST `/sijalak/v1/sessions/{session_id}/turns` | Binary canonical WAV with idempotency key, turn_id/generation and metadata; proposed 202 job ID/status; upstream key never returned | UNKNOWN | Input schema/content-type/idempotency/auth proof required; no paid duplicate admission |
| ROUTE-STATUS | GET `/sijalak/v1/sessions/{session_id}/turns/{turn_id}` | Device-owned bounded job status and validated reply metadata; 500 ms poll, deadline 30 s | UNKNOWN | Exact state/status/result schema and limit evidence required |
| ROUTE-AUDIO | GET `/sijalak/v1/sessions/{session_id}/turns/{turn_id}/audio` | Device-owned canonical normalized WAV; content length/type/checksum, bounded download, no arbitrary provider URL proxy | UNKNOWN | Exact HTTP status/content-type/checksum algorithm and real format proof required; no invented wire constants |
| ROUTE-DELETE | DELETE `/sijalak/v1/sessions/{session_id}` | Invalidate session/generation, stop playback, purge accessible transient state; cannot promise upstream deletion | UNKNOWN | Exact cancellation response/status/ownership/deletion contract required |

Proposed statuses/content types described semantically above are not actual response observations. Beyond the sourced proposed 202 turn acceptance, exact success/error status mapping, header names, checksum algorithm and serialization revision remain unfrozen rather than guessed.

## Schema, authorization and lifecycle matrix

All rows: SRC-ARCH-A §§4–6 and SRC-POLICY-A D-03/04/06/10/18, local 2026-10-09; public reference does not prove implementation; reported schema-specific value not supplied. Actual sourced behavior/revision/type precision UNKNOWN; required G1 proof is sanitized real contract/policy plus later separately authorized conformance checks.

| Contract field/group | Proposed value/policy (A) | Actual sourced value (V) | Required gate / later check |
|---|---|---|---|
| reply_text | Text ≤35 words; Bahasa Indonesia; bounded persona, no arbitrary firmware/tool commands | UNKNOWN | Source field type/counting rules; validate overlong/unsafe/invalid replies |
| action | Allowlist continue / handoff_book / safe_refusal / end_session | UNKNOWN | Strict server/controller validation; reject non-allowlisted actions |
| book_id | Nullable, member of current curated catalog when used | UNKNOWN; actual catalog also UNKNOWN | Authentic available catalog/ownership schema; reject invented/absent book IDs |
| schema_version | Versioned response field; exact identifier UNKNOWN | UNKNOWN | Custodian supplies revision and adapter compatibility record; no invented version |
| session_id / turn_id / generation | Opaque session ownership; bounded turn; generation correlates results and cancel/reset | UNKNOWN; concrete wire types UNKNOWN | Contract types and cross-session/stale-result checks before integration |
| Auth / credentials | High-entropy limited-scope device credential; hashed server lookup; rotation/revocation; no admin/provider key in firmware/URL/logs | UNKNOWN | Actual storage/device class/permission design proof; no credentials collected here |
| TLS / ownership | Server certificate verification; server enforces device class/mode and session/audio ownership | UNKNOWN | Actual TLS/auth/session contract and negative checks; never bypass TLS |
| Idempotency / retries | Duplicate new-turn submission must not duplicate paid inference; retry only under known safe policy | UNKNOWN | Real key/generation/deduplication semantics and quota/budget contract; no guessed upstream retry safety |
| Cancel / STOP / reset | Control deadline dominates completion; invalidate generation, stop output and purge accessible local/backend state | UNKNOWN | Exact cancellation/replay/storage rules plus authorized future tests; wrong/stale/cancelled generation never plays |
| Privacy / TTL | Metadata only, no raw audio/transcript/reply logs; TTL 10 min; audio result expires 60 s after completion | UNKNOWN | Actual processing/location/retention/deletion policy evidence; provider retention separate |
| Child isolation | Separate adult/child artifacts and backend permissions; child-local no capture/cloud/Gemini or training/generation route | UNKNOWN implementation | Independently verified technical isolation and eligible field permissions later; user age toggle cannot authorize child cloud |

## Pin, provider and processing-location register

This register distinguishes D public candidate strings from A role selections and V actual deployment. No installed version or owner account pin is accepted. Source date is 2026-10-09 prior research; no fresh retrieval. Service custodian is the required future evidence owner, not an assertion that evidence was supplied.

| Pin/stage | Public reference / recorded source (D) | Proposed value (A) | Actual sourced value (V) | Required evidence / gate |
|---|---|---|---|---|
| STT model | SRC-AI-D changelog: `gemini-3.5-transcribe`, recorded Aug 26, 2026 GA claim | Nonstreaming transcription role candidate | Exact deployed ID/version/account/availability UNKNOWN | Real offering/account contract/date/revision/language/format support; G1/G4 |
| Chat model | SRC-AI-D models page: `gemini-3.1-flash-lite` example | Economical bounded text-dialog role candidate | Exact deployed ID/version/account/availability UNKNOWN | Same, plus reply/catalog validation evidence; G1/G4 |
| TTS model | SRC-AI-D changelog: `gemini-3.8-flash-tts` and `gemini-3.8-flash-lite-tts`, recorded Sep 22, 2026 GA claim | Flash-Lite TTS role candidate, no silent substitute | Exact deployed ID/version/account/availability UNKNOWN | Actual offering/voice/price/locale/format evidence; G1/G4 |
| Language / voice | SRC-AI-D speech-generation indexed example uses prebuilt Kore; original live fetch timed out | Bahasa Indonesia; prebuilt voice, no cloning; exact voice not pinned | Indonesian STT/TTS support/quality and actual voice ID UNKNOWN | Account-specific support/contract and later conformance; G1/G4 |
| SDK/backend | SRC-AI-D prior https://fastapi.tiangolo.com/async/ public read | Isolated async FastAPI/Python 3.12, Google GenAI role adapters; blocking codec work isolated | Exact package/SDK/Python patch/build/deployment UNKNOWN | Genuine version/lock/toolchain record and package-legitimacy/separate install gate |
| Firmware/display | SRC-AI-D prior https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/i2s.html title v6.1; representative OLED C2 only | ESP-IDF 6.1.x; actual driver selected later | SDK patch/compiler/toolchain/driver/GPIO/clock/DMA/raw conversion UNKNOWN | Actual board/controller plus exact version/driver evidence; G1 before build/flash |
| Private host/routes/schema | U availability only, no published private source | Isolated `/sijalak/v1` namespace and route matrix | Hostname, active routes, schema, transport, auth, interface revision UNKNOWN | Genuine sanitized current custodian docs; no service access here; G1 |
| STT/chat/TTS processing location | No owner deployment location proof | Robot → isolated VPS adapters → eligible provider → normalized robot output | Actual location per stage, region, data path, account offering UNKNOWN | Actual contract/data-flow/location evidence; G1/G4 |
| Rates / quota / FX / billing | SRC-AI-D price limitation: former preview example is NOT selected-model pricing | Rp100,000/month ceiling proposal only; fail-closed admission | Current unit rates/currency/FX/quota/tier/taxes/billing permission UNKNOWN | Dated exact model/account prices, FX methodology, usage caps and separate billing/call approval; G1/G4 |
| Provider retention / deletion | SRC-TERMS-D unpaid training/human-review possibility; paid security/abuse logs/transient global storage may remain | Minimize content, metadata-only local policy | Actual account/tier retention/deletion settings/support UNKNOWN | Authentic current offering terms/account policy; no provider zero-retention promise; G1/G4 |

SRC-AI-D source URLs for recorded indexed candidates: https://ai.google.dev/gemini-api/docs/changelog ; https://ai.google.dev/gemini-api/docs/models/gemini-3.1-flash-lite ; https://ai.google.dev/gemini-api/docs/generate-content/speech-generation ; https://ai.google.dev/gemini-api/docs/generate-content/transcribe . These are prior documented references, not fresh reads or proof of present account support. No former preview price is used for cost estimates.

## Failure and revision register — future checks, not executed cases

Every row is proposed from SRC-ARCH-A §8/§6 and SRC-VERIFY-A; actual behavior/status/error schema UNKNOWN. No fixtures, live requests or empirical PASS results were generated.

| Required future negative condition | Proposed bounded disposition | Required evidence / frozen until |
|---|---|---|
| Malformed, oversized, unsupported or silent/empty input | Reject before paid stages, bounded visible failure; no reinterpretation as canonical WAV | Actual validator/size/status contract and authorized codec checks; G1/G4 |
| STT empty/unsupported speech | Bounded repeat or end, no invented transcript | Authentic role/error schema and later tests |
| Unauthorized, revoked, wrong device/mode or cross-session/audio access | Deny; no leaked result or child route | Ownership/auth/TLS policy and negative checks |
| Duplicate submission, stale generation, reset or cancellation | No duplicate paid inference; never play old/wrong-session audio; purge accessible transient state | Exact idempotency/generation/deletion semantics |
| Poll deadline, no network, TLS failure, quota/rate exhaustion | Bounded local error/end, no unlimited capture/retry; no TLS bypass | Real timing/rate/status rules and later fault tests |
| TTS unavailable/wrong format/oversize output | End or explicitly disclosed local fallback, excluded from AI-success counts | Actual codec/status/checksum/download contract |
| Invalid action/schema/book, injection, unsafe/overlong reply | Validate/reject or short safe refusal/end; no arbitrary commands | Actual schema/catalog/policy/guardrail evidence |
| Budget unconfigured, unknown rates, exhausted cap | Deny provider admission; no presumed free tier/billing auto-enable | Pricing/FX/account/usage authorization and admission evidence |

For mismatch, source a sanitized current value/date/revision, keep old D/U/A separate, record discrepancy and escalate to MAIN for approved adaptation. No silent pin/provider/driver/schema substitution or permission expansion. Missing proof keeps G1 integration/model/driver freeze and G4 live use BLOCKED.
