# Phase 1: Baseline Contracts and Safety — Specification

**Created:** 2026-10-09. **Status:** revision 2 DOC-FIRST functional documentary scope locked by owner selection “Revisi doc-first (Recommended)”; MAIN must bind AUTH-U-02 to the revised hash before dispatch. No new approval question.
**Requirements:** BAS-01, BAS-02, BAS-03 locked for planning, not empirical validation or implementation authority.
**Method:** autonomous choices from canonical sources; no interview or ambiguity score claimed.

## Goal
Maintainers can see actual inventory/contracts and which operations are permitted before building anything, as defined in `.planning/ROADMAP.md` Phase 1.

## Background
- U: electronics are owned: ESP32-S3 N16R8, OLED 1.3-inch 128×64, INMP441, MAX98357A/3W speaker, microSD, approximately 450 mAh 3.7V LiPo and charging/conversion modules. Variants, condition and compatibility are V.
- U: VPS STT/chat/TTS are reported available; “Gemini, presumably” does not establish provider, account, model or service contracts. Hostname is UNKNOWN; `/sijalak/v1` is proposed, not active.
- D: canonical Gemini Developer terms restrict under-18/child-directed or likely-under-18 clients. Adult internal R&D eligibility is V; parent/proxy/paid access is not an exception.
- U: Rp500,000 covers extra materials/power/case, not owned electronics, printer procurement/access, fieldwork or VPS/API operations; no spending is approved.

## Requirements — Current / Target / Acceptance
1. **BAS-01 — Provenance-labelled inventory and contracts.**
   - Current: owner reports and design parameters, without actual module measurements or inspected service evidence.
   - DOC-FIRST target: sanitized source-linked U ownership reports, D public references, A proposals and actual UNKNOWN labels/W×D×H/controller/service fields, distinguishable in inventory/contracts. Original actual-measurement proof remains required for BAS implementation/G1, not document completion.
   - Acceptance: every owned item has provenance, source receipt, units/method and status; unreadable/inaccessible facts stay V and required unresolved facts prevent a verified freeze.
2. **BAS-02 — Budget, power and permission boundaries.**
   - Current: Rp500,000 extra-material ceiling, unverified LiPo/power path and owner-approved bounded DOC-FIRST authority pending MAIN revision binding; external operations remain ungranted.
   - Target: separate material/operating ledgers, power-risk disposition and operation-specific authorization register.
   - Acceptance: 250k case + 100k controls/power/fasteners + 150k reserve = 500k allocation is labelled A; actual quotes/costs remain evidence-linked; unknown pricing, power safety or permissions cannot admit use.
3. **BAS-03 — Integration contracts and revision gates.**
   - Current: proposed REST job protocol, WAV normalization, limits and candidate stack/model roles; no active route or pinned account support proven.
   - Target: versioned proposed-versus-actual contract matrix covering STT/chat/TTS, auth, limits, errors, cancellation, privacy and package/model/voice/cost pins.
   - Acceptance: sanitized genuine contract sources or explicit V entries support every field; mismatches and missing pins keep integration/live-call gates closed without fabricating cases or results.

## Boundaries
- In scope under MAIN-bound AUTH-U-02: documentary use of existing authenticated owner reports/current project records and recorded public documents, sanitized reconciliation, cost/power/permission exclusions and phase-local readiness report. Physical inspection/measurement is not required or authorized by this revision.
- Future outputs: `01-AUTHORIZATION.md`, `01-INVENTORY.md`, `01-CONTRACTS.md`, `01-SAFETY-BUDGET.md`, `01-VERIFICATION.md`, `01-01-SUMMARY.md` in this phase directory; none is produced by this planning task.
- Out of scope: firmware/backend implementation, runtime/API tests, CAD/STL/slicing/printing, precise PCB mounts/GPIO/wiring, participant contact and fieldwork; other phases own those activities.
- MAIN owns root documents/config; this phase records recommendations without editing them. Existing global OpenCode, Pro and 9Router remain untouched.

## Constraints
- U/D/A/V labels are evidence classes, not verdicts; each A has a rationale and a V evidence/revision gate.
- D-04: input WAV PCM16 LE mono 16kHz ≤8s/256,044 canonical bytes; output PCM16 mono 24kHz ≤15s, download cap 768KiB. Raw I2S is not automatically canonical WAV.
- D-03/06/09/10: PTT plus separate STOP, half-duplex, REST polling 500ms/deadline 30s, separately 3 status requests/s versus 5 new turns/minute, Bahasa Indonesia, ≤35-word replies, ≤4 turns/180s; these are planning bounds. Actual 24/32-bit DMA/raw-slot format and conversion remain UNKNOWN, not canonical PCM16 proof.
- D-05/07/08: isolated async FastAPI/Python 3.12, ESP-IDF 6.1.x, role adapters; package patch/model/voice/rates require evidence before pins are accepted, not installations here.
- D-13/14: total height 140–160mm, nominal 150mm, torso 112×102×122mm and 70×55×80mm clear reservation are A design inputs; PLA 130–170g/cap 170g PER complete case set includes its support/brim/purge/waste, not assembled mass or fit proof. Coupons/reprints have a separate ledger and count in cumulative extra-material cash.
- D-15/16/17/18: verified regulated external power first; proposed operating ceiling Rp100k/month is unapproved; adult-lab and child-local permissions/artifacts remain separate.

## Acceptance Criteria — Future Evidence Only
- [ ] BAS-01/02/03 map to sanitized source-backed records and unresolved gates, with no planning/parser pass counted as implementation.
- [ ] MAIN binds the already supplied DOC-FIRST owner selection to AUTH-U-02 and actual new 01-01 hash before dispatch; no repeated question. Installs, server/VPS access, API calls, billing, field contacts, printing, physical inspection and Git remain ungranted.
- [ ] Actual labels/dimensions and inspected contract evidence are distinguished from proposals; missing facts do not receive verified status.
- [ ] Adult eligibility, power, costs, privacy and model/interface revisions have named owners/required evidence and fail-closed gates.

## Edge Coverage
| Edge | Requirement | Planned handling / future evidence |
|------|-------------|------------------------------------|
| Unreadable label, conflicting dimensions/controller | BAS-01 | Record V/conflict and inspection method; no derived fit/wiring claim. |
| API/model/account differs from report or proposed protocol | BAS-01/03 | Separate actual and proposed fields; document adapter/revision gate before integration. |
| Missing prices, printer access, power protection or permission | BAS-02 | Deny expenditure/use; source-backed resolution required, not an assumed safe/free path. |
| Child access, stale reply, oversized audio or unknown retention | BAS-03 | Document isolation/ownership/cancel/size/retention contracts; runtime checks belong to authorized later phases. |

## Prohibitions
No invented labels, measurements, quotes, contracts, test cases or PASS results; no secrets/private identities; no child Gemini/audio egress or consent/proxy/tier workaround; no code/CAD/layout or unauthorized operations.

## Assumptions and Future Gates
A: phase-local Markdown records provide inspectable documentary handoff; MAIN verifies source completeness. Owner DOC-FIRST selection supplies fresh approval; MAIN's AUTH-U-02 hash binding precedes dispatch. G1 actual inventory/contracts/pins and later G3/G4 remain BLOCKED. Existing local sources support a genuine provenance tracer; no actual OLED packet is fabricated. UNKNOWN actual fields allow documentary completion but never freeze or BAS proof. Missing/contradictory documentary sources block the affected task. SUMMARY only after documentary tasks complete, with documentary_work_complete: true, readiness: BLOCKED, phase_goal: NOT_VERIFIED, requirements-completed: []; MAIN owns independent BAS/phase/root decisions. No has_summary shortcut. See 01-REVISION.md; historical physical prerequisite is superseded for DOC WORK ONLY.
