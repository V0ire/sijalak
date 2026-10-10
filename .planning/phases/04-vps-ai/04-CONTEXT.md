# Phase 04: VPS Speech Gateway and AI Guardrails — Context

Gathered: 2026-10-09. YOLO planning only; choices below are not implementation authorization.
Source precedence: current user instruction → canonical root decisions/contracts → recorded research → local planning choices.

<domain>
## Phase boundary
Restricted adult backend E2E after Phase 1; no operational companion and no child Gemini client. Two plans: 04-01 tracer/security, then 04-02 privacy/faults/evals.
</domain>

<decisions>
## Decisions — numbered, with rationale
Global D IDs retain the meanings in `.planning/DECISIONS.md`; D-19…D-22 capture this instruction's additional locks.
| ID | Decision | Rationale / evidence boundary |
|----|----------|-------------------------------|
| D-01 | Eligible restricted internal adult R&D only; no child Gemini through proxy/guardian/paid tier or adult relay | Recorded Developer API terms restrict users and client audience; eligibility still unverified |
| D-04 | Input PCM16 WAV16k mono ≤8 s/256,044 bytes; output WAV24k mono ≤15 s/720,044 bytes, hard 768 KiB | Canonical conversion prevents provider/microphone codec assumptions |
| D-05 | Isolated async Python 3.12/FastAPI gateway and role adapters; one-worker transient jobs, SQLite metadata only | Avoid blocking event loop and touching existing VPS/OpenCode/9Router services |
| D-06 | REST jobs under `/sijalak/v1`, host UNKNOWN, polling 500 ms/deadline 30 s | Bounded inspectable sequential flow; no default WebSocket |
| D-08 | STT/chat/TTS candidates 3.5-transcribe/3.1-flash-lite/3.8-flash-lite-tts behind verified role pins | Indexed official evidence is not account/API/price approval; no silent model substitution |
| D-09 | Bahasa Indonesia and provisional prebuilt Kore; no cloning | Locale/voice conformance required before live use |
| D-10 | ≤4 turns/180 s and ≤35 words; bounded repeat/end and attention handoff | Limits robot attention, audio and cost without claiming literacy benefit |
| D-17 | Rp100k/month planning ceiling; unknown rates/FX/approval deny calls | Hardware Rp500k is separate; do not invent prices or enable billing |
| D-18 | Separate deployment identities and backend permission; token/mode flags cannot upgrade child client | Binary separation and server-side authority are both necessary |
| D-19 | Each named plan needs new explicit implementation approval before workflow starts; other permissions stay separate | Current permission covers Markdown planning only; never inherit approval from another plan |
| D-20 | Unique per-device hashed auth, TLS verification, session ownership, idempotency, ≤5 turns/minute | Prevent replay, cross-device access and duplicate paid work; default-deny capability |
| D-21 | No raw-content logs/profiles; transient TTL10 min/audio expiry60 s; cancel/reset discards generations | Minimize collection and stale exposure; local/backend cleanup does not delete provider records |
| D-22 | Small DEMO vs actual-TBM catalog and schema/allowlist/content validation before TTS/actions | Prompt alone cannot stop invented books or unsafe controller instructions |
### Agent discretion
Internal names/test organization within these contracts; selected queue policy is one global active job, one/device and no pending queue for the one-device demo. No discretion to change eligibility, provider, budgets, permissions or limits silently.
</decisions>

<canonical_refs>
## Canonical references
- `.planning/PROJECT.md`, `.planning/DECISIONS.md`, `.planning/REQUIREMENTS.md`, `.planning/ROADMAP.md` — provenance, requirements and dependencies.
- `.planning/ARCHITECTURE.md` §§1–6,8; `.planning/AI-SPEC.md` §§2–9 — isolation, routes, audio, policy, retention and evals.
- `.planning/USER-FLOWS.md` A/C; `.planning/MVP.md`; `.planning/VERIFICATION-STRATEGY.md` — actual versus partial success and authority gates.
- `.planning/research/AI-RESEARCH.md`; `.planning/phases/04-vps-ai/04-SPEC.md` — recorded evidence and current/target acceptance.
</canonical_refs>

## Existing inputs / future facts
No backend was inspected, created or tested. Owner reports “unverified Gemini, presumably”; actual offering/host/auth/schema/account/rates/locale remain V.
Future Phase 1 supplies sanitized inventory/contract/permission evidence. Unmet eligibility stops provider calls; unknown host must not be contacted.
Future permissions: server/access, dependency install, provider/data, billing/spend, Git and printing are distinct from named-plan G0 approval; none granted.
Commands in plans are prospective only; no CLI/init/query/hooks/graph/research/test/build/API/Git operation ran. No calibration evidence is invented.

<deferred>
## Deferred ideas
Device tracer (05), assembled demo/safety (06), field catalog/pilot (02/07), enclosure/printing (03/06); no RAG/web agent, streaming default, voice cloning or persistent visitor profiles.
Child live AI needs a separately eligible offering/contract and new review; it is not delivered by these plans.
</deferred>
