# Phase 1: Baseline Contracts and Safety — Context

**Gathered:** 2026-10-09 from supplied canonical documents, without questions.
**Status:** revision 2 DOC-FIRST documentary scope owner-approved by “Revisi doc-first (Recommended)”; MAIN AUTH-U-02 hash binding required before dispatch. No hardware/API implementation or empirical validation authorized.
U = owner report; D = documented constraint; A = selected assumption; V = pending evidence. D-NN below is a decision identifier, not an evidence verdict.

<domain>
## Phase Boundary
Phase 1 supplies inspectable actual inventory/contracts and permission, power, cost and integration gates for BAS-01/02/03. ROADMAP dependency: none. MAIN retains root/config ownership; all execution artifacts are future phase-local Markdown.
</domain>

<decisions>
## Decisions — Locked Planning Baseline
The numbered canonical decisions listed below retain their register meanings; D-19/D-20 are phase-local selections, not additions to the root register. Other canonical decisions belong to the deferred phase scopes, not new requirements here.
- **D-01 [D→V]:** separate restricted internal adult R&D from child-local work; eligibility before calls. Rationale: provider restrictions, not field evidence; guardian/proxy/paid access cannot bypass them.
- **D-03 [A→V]:** half-duplex PTT and separate STOP. Rationale: bounded capture, echo/privacy control; actual buttons/usability remain unknown.
- **D-04 [A→V]:** input PCM16 LE mono 16kHz/8s; output PCM16 mono 24kHz/15s, normalized on VPS. Rationale: explicit inspectable audio contracts, not assumed microphone/provider compatibility.
- **D-05 [A→V]:** isolated async FastAPI/Python 3.12 adapter boundary. Rationale: preserve existing services/global OpenCode/Pro/9Router rather than replacing infrastructure.
- **D-06 [A→V]:** REST jobs, bounded polling/download/cancel, not streaming. Rationale: inspectable errors; `/sijalak/v1` remains proposed and hostname UNKNOWN.
- **D-07 [A→V]:** ESP-IDF 6.1.x baseline; patch/toolchain/controller pins pending inventory. Rationale: direct S3/I2S control, with no SDK install implied.
- **D-08 [D/A→V]:** STT/chat/TTS role adapters with provisional 3.5 Transcribe, 3.1 Flash-Lite and 3.8 Flash-Lite TTS candidates. Rationale: pin verified exact IDs/account support/prices before use; do not silently substitute.
- **D-09 [A→V]:** Bahasa Indonesia and prebuilt voice. Rationale: constrain unverified language/voice complexity; no cloning.
- **D-10 [A→V]:** ≤4 turns/180s and ≤35-word replies. Rationale: attention, audio and cost bounds, not measured performance.
- **D-13 [A→V]:** nominal 150mm, torso 112×102×122mm, four case pieces; collect envelopes/service constraints only. Rationale: measured parts must constrain later design; no precise mounts/wiring here.
- **D-14 [U/A→V]:** PLA target 130–170g, cap 170g including all parts/support/brim/purge/waste. Rationale: distinguish material consumption from assembled mass; no fit/slicer proof inferred.
- **D-15 [A→V]:** verified regulated external supply first; battery/protection/power path remain gated. Rationale: owned 450mAh LiPo does not prove runtime or safety.
- **D-16 [U/A→V]:** 250k case + 100k controls/power/fasteners + 150k reserve within Rp500k extra-material cap. Rationale: explicit allocations, not quotes; printer/access/fieldwork/operations separately disclosed.
- **D-17 [A→V]:** proposed Rp100k/month operating ceiling, separate rates/FX/usage/account/billing authorization. Rationale: hardware budget cannot fund presumed free or unbounded API use.
- **D-18 [A→V]:** separate adult-lab/child-local deployment artifacts and backend permissions. Rationale: controls/age toggles cannot unlock child cloud AI; technical enforcement belongs to later phases.
- **D-19 [U→V binding]:** owner explicitly selected DOC-FIRST revision/execution of 01-01; MAIN binds AUTH-U-02 to actual revised hash before dispatch, without asking again. Historical AUTH-U-01 binds revision 1 only. Rationale: named-plan scope/revision authority cannot be inferred; each external operation retains a separate gate.
- **D-20 [A→V]:** use phase-local source IDs, U/D/A/V rows, sanitized receipts and UNKNOWN/conflict dispositions. Rationale: evidence can be audited without secrets/identities; missing facts cannot become invented validation.

### Agent's Discretion
Choose document table structure and harmless evidence IDs without new dependencies. Preserve canonical limits; record conflicts/revision proposals for MAIN instead of editing roots or claiming accepted pins.
</decisions>

<canonical_refs>
## Canonical References
- `.planning/PROJECT.md` §§ Arahan pengguna, Gate provider, Otorisasi — ownership, reports, permissions and unknowns.
- `.planning/DECISIONS.md` §§ Evidence labels, User decisions, Auto-selected planning decisions — canonical U/D IDs and rationale.
- `.planning/REQUIREMENTS.md` § Baseline and contracts; `.planning/ROADMAP.md` § Phase 1 — BAS scope, goal, dependency and acceptance.
- `.planning/ARCHITECTURE.md` §§ 1–4, 6–7 — proposed interfaces, limits, data, cost and power boundaries.
- `.planning/MVP.md` §§ Release / implementation gates, Budget; `.planning/DISCOVERY.md` §§ Izin, Catatan — adult/local split and field-data exclusions.
- `.planning/VERIFICATION-STRATEGY.md` §§ Evidence / gates, Reporting — future evidence is not planning PASS.
- `.planning/phases/01-baseline-contracts/01-SPEC.md` — phase acceptance contract.
</canonical_refs>

## Existing Evidence / Integration Points
No inspected runtime/code/module contract evidence was supplied. Existing authenticated owner reports/current project records and recorded public research ARE available local documentary sources: U ownership/availability, D Gemini Terms/Waveshare reference, A packaging/interface proposals. These support a real provenance tracer without physical inspection; actual owner controller/variant/measured envelope, service host/schema/account/pins remain UNKNOWN. Public dimensions never enter actual columns. Sanitized records feed later gates, not release phases 3/4/5; G1 BLOCKED. No endpoint is activated here.

## DOC-FIRST revision contract
01-REVISION.md supersedes the historical Task 2 physical prerequisite for DOCUMENTARY WORK ONLY. AP-01 remains binding as no source promotion and readiness gate before CAD/integration. Missing/contradictory documentary source support blocks work; UNKNOWN actual fields do not. Tasks can complete documentary records and then SUMMARY with requirements-completed: [], readiness: BLOCKED and phase_goal: NOT_VERIFIED. MAIN owns receipts/checkpoint/continuation, root updates and independent actual BAS/phase completion; no has_summary shortcut. Existing AUTH-U-01/checkpoint/continuation stay historical and are not rewritten here.

<deferred>
## Deferred Ideas
CAD/print/precise PCB mounting and wiring (phase 3), gateway/runtime model evaluation (phase 4), firmware/profile enforcement (phase 5), safety/demo tests (phase 6), discovery/pilot field execution (phases 2/7). Always-listening, motors, RAG, profiles, cloned voices and live child Gemini remain excluded/deferred; this plan does not implement them.
</deferred>
