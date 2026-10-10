# Phase 8: Iteration and Handover — Specification

**Created:** 2026-10-09. **Status:** planning only; MAIN/user review pending; execution not authorized.
**Requirements:** OPS-01, OPS-02. **Depends on:** Phase 7; prerequisites are future evidence, not assumed completion.
**Ambiguity assessment:** not performed; no interview, numeric score, test result, or approval is asserted.

## Goal
Owner/operator can maintain an evidence-labelled release and decide continue/revise/stop using actual Phase 7 findings, safe operating instructions, and accountable maintenance responsibilities.

## Current state
- Canonical documents define proposed profiles, controls, safety and evaluation gates; no as-built release, operator receipt, or operational validation has been established here.
- Reading-start assistance is a hypothesis. Pameungpeuk is the initial region; no TBM, partner, baseline observation, or participant permission is confirmed now. Cohort 10–14 is provisional.
- Phase 7 reports and prerequisite evidence are future inputs supplied through MAIN; missing inputs must remain unknown, not become a favourable outcome.

## Requirements — Current / Target / Acceptance
1. **OPS-01 — Safe, maintainable operator package**
   - Current: procedures and responsibilities are proposed; actual controls, release versions, safe power path and accountable maintainer are unconfirmed.
   - Target: locally reviewable operator, maintainer and incident guides tied to actual release/profile/content revisions, covering startup, STOP, failure, recovery, content/profile updates, cleaning, storage and power/charging restrictions.
   - Acceptance: each procedure states prerequisites, observable result, failure/stop boundary and responsible role; owner/operator receipt and maintainer acknowledgement are recorded or explicitly pending, blocking handover acceptance.
2. **OPS-02 — Evidence-led iteration decision**
   - Current: no actual pilot findings, effect threshold, rates/FX/quotas or rollout readiness are established by this planning task.
   - Target: sanitized evidence review and revision register compare actual Phase 7 robot/no-robot observations, denominators, assistance, distraction, burden and unresolved risks; proposed continue/revise/stop receives owner review.
   - Acceptance: every conclusion links to actual evidence and its limitations; missing evidence produces blocked/unknown findings. All-phase quality criteria are cross-referenced without claiming those phases passed. Continue never grants rollout authority.

## Boundaries
- In scope after named approval: Markdown operator/maintainer/incident package, as-built reference manifest, evidence review, bounded documentation revisions, revision register and truthful phase verification/summary.
- Current output only: `08-SPEC.md`, `08-CONTEXT.md`, `08-01-PLAN.md`; none is an executed handover report.
- Out of scope: implementing release changes, deploying/updating devices or servers, rotating credentials, purchasing, fieldwork, CAD/slicing/printing, automated commits/tags, remote Git or publishing secrets. Root planning/state/config/roadmap updates belong to MAIN.

## Constraints
- G0 requires new explicit approval naming 08-01 before executor dispatch; the first task rechecks it with a blocking-human decision. YOLO is planning permission only.
- Adult D3 remains restricted internal 18+ R&D, subject to actual provider/account/contract eligibility. Child-local uses only human-authored/recorded/licensed local content, no microphone capture, Gemini authoring, cloud AI or voice egress; parent/proxy/paid/age controls are not exceptions.
- Document actual signed/deployment profile versions and backend privilege separation; an operator control must not unlock adult AI from child-local. Any expanded child AI needs a new eligible contract plus threat/evaluation review before use.
- Preserve bounded PTT/STOP and honest local-fallback labels; no RAG, identity profiles, cloned voice, always-listening, reward loop, reading diagnosis or unverified comprehension/habit claim.
- Extra materials ceiling Rp500k excludes owned hardware, printer procurement and operating/API fees. Rp100k/month is an operating placeholder, not spending approval; actual role rates, billed units, FX, quotas and authorization remain unknown until evidenced.
- Original hybrid Jalak Gembul: nominal H150 within 140–160 mm, four pieces, standard PLA consumption target 130–170 g including support/brim/purge, cap 170 g; not assembled weight or proof of manufacture/fit. No invented internal mounting/wiring layout.
- Maintenance must retain actual revisions, measured envelopes, material/profile provenance, support/waste totals and heat/fit evidence. External regulated power requires evidence; unverified battery/charger/power-path operation remains disabled, not made safe by instructions alone.

## Acceptance criteria — future, unchecked
- [ ] AC-01: named implementation approval and actual Phase 7 evidence paths exist; approvals for other operations remain separate.
- [ ] AC-02: guides trace one evidence-backed startup → local book handoff → STOP → shutdown/storage path without claiming a new device test.
- [ ] AC-03: failure/incident matrix covers network/auth/quota, stale playback, content/profile mismatch, privacy, heat/power/mechanical concerns and unavailable maintainer; stop/escalation/restart responsibility is explicit.
- [ ] AC-04: content provenance/licence/catalog and profile compatibility/rollback procedures use actual versions; unknown or unsafe rollback keeps the unit stopped, never enables cloud AI on child-local.
- [ ] AC-05: as-built references and all-phase quality/evidence rows distinguish passed, failed, partial, blocked and unknown; proposed limits are separated from observations.
- [ ] AC-06: actual Phase 7 comparison and limitations support a review-pending continue/revise/stop recommendation; missing denominators/criteria do not become evidence of benefit.
- [ ] AC-07: operator receipt, accountable maintainer acknowledgement and owner decision are factual or pending; unresolved safety/eligibility/budget/review gates withhold operational handover/rollout.

## Edge coverage — planned resolutions, not tested outcomes
| Edge | Requirement | Required resolution |
|---|---|---|
| Phase 7 blocked, absent or inconclusive | OPS-02 | Accept a truthful stopped/blocked report as evidence; halt dependent drafting if no report exists; do not infer benefit or rollout. |
| Wrong profile, obsolete content or unsafe rollback | OPS-01 | STOP, isolate/quarantine, use only a compatible proven revision after separate authorization. |
| Heat, swelling, smell, instability or failed STOP | OPS-01 | Withdraw from use; apply evidence-backed safe power procedure; qualified maintainer review before restart. |
| Unknown prices, unavailable books or missing owner | Both | Keep live calls/content/use blocked as applicable; assign an unresolved issue without inventing price, title or contact. |

## Prohibitions
- Must not turn planning permission, an adult demo, document checks or a continue recommendation into deployment/field/Git/spending approval.
- Must not contact providers/participants/VPS, read credentials, publish secrets or change global OpenCode/Pro/9Router/existing VPS services in this phase's documentation work.
- Must not fabricate data, sign-offs, as-built versions, physical print claims, internal construction or successful phase completion.

## Assumptions and references
Local sanitized inputs and owner/operator acknowledgements may become available later through MAIN; their identities, paths, values and availability are unresolved now. No fresh collection is authorized to fill gaps. Planning choices are recorded without an interview.
Sources: `.planning/{PROJECT,DECISIONS,REQUIREMENTS,ROADMAP,ARCHITECTURE,MVP,VERIFICATION-STRATEGY,INDUSTRIAL-DESIGN,AI-SPEC,DISCOVERY}.md`; local GSD `spec.md`, `context.md`, `phase-prompt.md`. See `08-CONTEXT.md` for decisions and `08-01-PLAN.md` for future work/gates.
