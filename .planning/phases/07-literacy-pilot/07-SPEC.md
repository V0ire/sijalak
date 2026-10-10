# Phase 07: Literacy Pilot and Evaluation — Specification

**Created:** 2026-10-09. **Status:** planning only; implementation, fieldwork and findings pending.
**Requirements:** PIL-01, PIL-02, PIL-03. **Dependencies:** accepted Phase 2 and Phase 6 evidence plus child-profile/provider gate.
**Method:** local-source planning with owner constraints; no interview, new research, ambiguity score or completion evidence claimed.

## Goal
Learn whether a local curated character intervention helps actual reading starts without provider/privacy violations, as specified in `.planning/ROADMAP.md` Phase 7.

## Background
- Region: Pameungpeuk, Kabupaten Bandung; no named TBM partner, access agreement, observed visitors, checked catalog or pilot results exist.
- Helping visitors START reading is a temporary product hypothesis; root problem remains unknown. Self-initiation is a preference with operator support, not validated unsupervised use.
- Cohort 10–14 is provisional: actual attendance, reading ability without diagnosis, language and accessibility evidence must validate or revise it.
- Recorded Gemini Developer API terms prohibit clients directed toward or likely accessed by under-18s. Phase 6 D3 is a separate restricted adult demo, never field-benefit evidence.

## Requirements — Current / Target / Acceptance
1. **PIL-01 — Permissioned, technically isolated child-local pilot.**
   - Current: planned profile and permissions only; no approved partner, guardian consent, child assent or measured readiness.
   - Target: actual site/institution permissions and approved participation/data process; distinct child-local binary, separate backend permission, verified adult-token removal/revocation and safe device before access.
   - Acceptance: sanitized gate and actual device/permission/credential/mic/egress evidence precede participant access; no voice capture, provider credentials, Gemini or other cloud-AI calls; refusal/withdrawal never punished.
2. **PIL-02 — Real, maintainable local book invitation.**
   - Current: hook-plus-choice hypothesis; no child scripts/audio or verified available books supplied here.
   - Target: human-authored/original, human-recorded or appropriately licensed hooks with at most three options per choice step from 6–12 actual available books; original visitor book choices remain free.
   - Acceptance: rights/source review and shelf availability support every active hook/book; offline START/STOP, operator update/disable and quiet handoff work; fixed local fallback is disclosed, never presented as AI.
3. **PIL-03 — Matched exploratory evidence, not efficacy proof.**
   - Current: proposed proxy/comparator/sample only; actual findings pending.
   - Target: same books, invitations and attention/help availability in robot and no-robot card/relawan conditions; proposed 10–20 eligible opportunities per condition over ≥2 periods, counterbalanced where feasible.
   - Acceptance: report all permitted eligible/invitation/exposure/opt-in/start/help/context denominators, elapsed time to book, distractions, queues, operator burden, refusals/harms and missingness; never all baseline visitors versus self-selected robot users.

## Boundaries
- In scope after fresh permissions: local-device readiness rehearsal, approved pilot protocol, consented matched observations and honest aggregate evaluation supporting continue/revise/stop.
- Exactly one plan, `07-01-PLAN.md`; wave 1 and empty dependencies are phase-local scheduling, not a waiver of phases 2/6 or child-profile gates.
- Future outputs: authorization, protocol, readiness, aggregate observations, evaluation, verification and summary named in the plan; now only SPEC, CONTEXT and PLAN.
- Out of scope: Phase 6 adult D3, firmware/backend repairs, new provider integration, CAD/art/printing or agent-generated child assets, diagnosis, comprehension/habit claims or powered efficacy study; permitted human media is supplied separately, never generated now.

## Constraints
- Fresh explicit owner approval for THIS named plan must precede native executor/workflow startup; contact, site/body approval where applicable, collection, guardian consent and ongoing child assent are separate gates.
- Device operation, server/API/authentication/billing, installs, Git and printing have separate permissions; planning or pilot consent grants none of them automatically.
- Child-local operates without assumed internet, capture or cloud-AI route. No age checkbox, proxy, adult relay, VPS or paid-tier workaround; no Gemini child-script authoring pipeline.
- Proposed reading-start proxy: ≥30s voluntary engagement with book content, including assisted/shared reading; agree observable criteria beforehand, separate merely holding/opening and robot play, and make no comprehension inference.
- Short local hook plus limited choice gives space to reading; proposed handoff within 90s is an interaction target, not reading proof. No scores, streaks, reading timer or identity profile.
- No names, DOB, schools, photos/audio or participant IDs in the versioned repository. Consent records and temporary notes stay operator-protected outside the project under an approved access/retention/deletion process; publish only disclosure-reviewed anonymized aggregates.
- Actual print/fit, closure/stability, acoustic/volume, STOP, power and thermal evidence must pass before child use. Battery/charging operation stays blocked unless independently verified and authorized; stop early for harm or overload, never to satisfy quotas.

## Acceptance Criteria — Future Only
- [ ] PIL-01 gates precede access; separate binary/backend permissions, credential removal/revocation, no-mic/no-egress and physical safety checks match the actual pilot unit.
- [ ] PIL-02 uses 6–12 real available books and reviewed human/licensed content; operator update/disable, offline start/stop, honest fallback and quiet handoff are demonstrated.
- [ ] PIL-03 protocol precedes collection; matched denominators, assistance/context, counterbalance deviations, missingness, adverse events and opportunity-not-person counts are reported without promised uplift.
- [ ] Results remain pending until authorized evidence exists; aborted/blocked work is labelled as such, not successful completion.

## Edge Coverage
| Edge | Required handling |
|------|-------------------|
| Missing permission, absent guardian consent, declined/revoked assent | No affected access/collection; approved aggregate gate status only; normal book/TBM access preserved. |
| Different cohort/ability/access needs, low attendance or unequal periods | Revise protocol before collection with approvals; report deviations and limits, no forced quota or invented sample. |
| <6 suitable books, removed/corrupt hook, unclear rights or empty media | Block readiness or re-curate; disable affected item, ask-human/quiet local stop, no invented holdings or cloud fallback. |
| Wrong binary, resident/reused adult token, mode spoof, capture/egress | Remove unit from access; require authorized predecessor remediation and actual re-verification, never a UI age gate. |
| Heat/instability/noise/distress, fixation/queues or operator overload | Stop affected session/period immediately; document permitted aggregate incident/burden and reconsider continuation. |

## Prohibitions and Uncertainties
No current contact/collection, covert observation, coercion, generated child text/audio/images, live child AI, private repo records or fabricated results. Partner, cohort, holdings/rights, proxy suitability, decision thresholds, attendance, operator capacity, retention and unit safety/isolation remain unverified; approval and evidence—not invented scores—resolve them.
