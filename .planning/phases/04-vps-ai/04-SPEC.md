# Phase 04: VPS Speech Gateway and AI Guardrails — Specification

Created: 2026-10-09. Status: planning only; no execution authorized or evidence of implementation.
Dependencies: Phase 1 BAS-01/BAS-02/BAS-03 acceptance before implementation integration.
Requirements: API-01, API-02, API-03, API-04, AI-01, AI-02, AI-03, AI-04, AI-05, AI-06.

## Goal
A verified isolated adult internal R&D voice turn can be processed and bounded without altering unrelated services or exposing secrets.
Phase 04 proves the backend path; Phase 05 adds the actual robot microphone/speaker path; Phase 06 establishes assembled demo readiness.

## Current → target → acceptance
All current entries below are unverified owner reports or planning contracts, not existing working capabilities.
| ID | Current | Target | Acceptance after authorization |
|----|---------|--------|--------------------------------|
| API-01 | VPS/API and speech stages reported available | Isolated Python 3.12/FastAPI async STT→chat→validator→TTS job | Actual approved adult WAV produces downloadable canonical audio through all real adapters; mock run labelled partial |
| API-02 | Auth/host/ownership uninspected | Unique device auth, TLS, server-side permissions, ownership, deduplication and limits | Invalid/revoked auth denied; cross-device access denied; concurrent duplicate incurs one inference sequence |
| API-03 | No reset/retention evidence | Cancellation generation, inaccessible stale results, bounded transient purge | Reset during every stage prevents retrieval/republication; TTL tests purge content; provider deletion not claimed |
| API-04 | Rates/metrics/errors unknown | Metadata-only stage/error/usage counters, fail-closed admission and deadlines | Timeout/outage/quota/unpriced cases terminate boundedly; counters reconcile without content logs |
| AI-01 | “Gemini, presumably”; offering unknown | Eligible restricted internal 18+ R&D client only | Contract/account approval precedes first call; child device/token/mode spoof never invokes a provider |
| AI-02 | Candidate models/voice/locale untested | Verified pinned role adapters and Indonesian canonical formats | Account/API/price/locale evidence precedes live requests; unsupported role stops without silent substitution |
| AI-03 | Persona/catalog/schema only specified | Short bounded reply, allowlisted actions/book IDs, safe persona | Invalid/injected/unsafe/unknown-book output never reaches action or TTS; DEMO facts not claimed as TBM holdings |
| AI-04 | No capture/data-path inspection | Explicit-capture attestation, no raw-content logs or persistent visitor profiles | Fixtures inspect logs/storage and lifecycle; real visible robot capture confirmed only in Phase 05 |
| AI-05 | No measured fallback/cap behavior | Disclosed local/error fallback, strict duration/word/turn/cost caps | Fallback excluded from AI-success counts; no uncontrolled retries or unlimited conversation |
| AI-06 | No dataset/results | Synthetic/consenting-adult cases and human rubrics with metadata evidence | Proposed ≥30 eligible turns and negative set reported with denominators, failures and scope; no invented scores |

## Boundaries
- In scope: complete backend tracer, device/session protocol, safe role validation, cancellation/privacy/budget controls and future adult evaluation evidence.
- Use explicitly marked DEMO catalog if Phase 2 is unavailable; actual TBM references require Phase 2 verified holdings. No Phase 2 dependency for the adult tracer.
- Out of scope: device firmware/physical safety acceptance (05/06), fieldwork/pilot (02/07), enclosure/printing (03/06), child live AI and unrelated service administration.
- Produces exactly 04-01 and 04-02 plans; does not change root documents, configuration, roadmap counts or execution progress.

## Constraints
- Host `UNKNOWN` is an unresolved configuration sentinel, not an active URL; every proposed route inherits `/sijalak/v1`.
- Input canonical WAV PCM signed16 little-endian/16 kHz/mono: ≤8 s, ≤256,000 sample bytes +44-byte header; reject inconsistent headers/content.
- Output canonical WAV PCM16/24 kHz/mono: ≤15 s, ≤720,000 sample bytes +44-byte header; absolute download cap 768 KiB.
- Sessions ≤4 accepted turns/180 s; reply ≤35 words; submit→completion deadline 30 s; poll interval 500 ms; new turns ≤5/device/minute.
- Transient session TTL ≤10 min; completed audio expiry ≤60 s; stop/reset/end purges accessible content earlier and invalidates generation.
- Candidates only: `gemini-3.5-transcribe`, `gemini-3.1-flash-lite`, `gemini-3.8-flash-lite-tts`, prebuilt `Kore`; exact availability/price/locale approval pending.
- Provider secret exists only on the authorized VPS; unique limited-scope device credential is not a provider/admin key. No borrowing OpenCode/Pro/9Router credentials.
- Operating Rp100,000/month is an unpriced planning ceiling, not spend permission; Rp500,000 additional hardware is separate. Missing prices/FX/billing approval deny live calls.
- One-worker bounded transient jobs; durable auth/permission/budget metadata may use SQLite, never durable voice/transcript history. No unverified multi-worker deployment.

## Acceptance criteria
- [ ] Named-plan implementation approval and separate external permissions exist before any workflow, dependency operation or network access.
- [ ] Real approved backend tracer and labelled deterministic mock path are distinguished; mock/fallback cannot satisfy live AI acceptance.
- [ ] Ownership, idempotency, rate/budget limits, reset races, expiry and hostile output fixtures meet the target table.
- [ ] Eval records include stage timing/error/cost denominators; robot-audible p95 and full-system success remain unverified until device evidence exists.

## Edge coverage / prohibitions
| Edge or must-NOT | Required future check |
|------------------|-----------------------|
| Under-18/child-directed Gemini, proxy/guardian/paid-tier workaround, relayed child speech | Offering/client eligibility review and child-credential/mode-flip denial fixtures; no calls for child script generation |
| Malformed/silent/oversized WAV; empty/unsupported STT; long/unsafe/schema-invalid reply | Bounded error/refusal; one explicit repeat opportunity then end; no imagined transcript |
| Stop during STT/chat/TTS/download; duplicate/reset/restart/revocation races | Generation checked before every stage/publication; content removed; restart invalidates transient sessions |
| Provider 429/outage/deadline; unknown prices; missing catalog | Fail closed, disclosed error/local outcome, no paid retry without known-safe policy, no fake holdings |
| Raw-content/credential logging, cross-visitor memory, arbitrary model commands/URLs | Metadata allowlist and storage inspection; reject outputs beyond reply schema |
| RAG/web tools/voice cloning/emotion or speaker identification; UI/code/assets now | Excluded; only future task descriptions and Markdown artifacts are produced |

## Assumptions and future facts
U: owner availability reports remain unverified. D: recorded Developer API terms require 18+ and exclude child-directed/likely-under-18 clients.
A: isolated stack, canonical formats, candidates and limits are selected contracts. V: actual host/schema/models/codecs/prices/locale/retention/latency/safety await approved evidence.
No ambiguity score, consent, measured result or provider exception is invented; unmet assumptions block the relevant future operation, not this planning deliverable.
