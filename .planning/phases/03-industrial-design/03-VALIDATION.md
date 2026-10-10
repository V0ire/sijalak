---
phase: '03'
slug: industrial-design
status: draft
nyquist_compliant: false
wave_0_complete: false
created: 2026-10-09
---

# Phase 03 — Validation Strategy

Supplement to existing 03-SPEC/CONTEXT/03-01/03-02 and root VERIFICATION-STRATEGY; all task/check outcomes are **pending**.
Strategy/mappings are planned; runtime scaffolds/evidence are not done. No app/tests, CAD/STL/3MF/G-code, slice, coupon, print or live proof is created now.
MAIN must validate Phase 1 inputs and NEW real owner messages for EACH named plan before executor startup; 03-01 cannot approve 03-02, and strings cannot approve either grant.

## Test Infrastructure
| Property | Value |
|----------|-------|
| Framework / config | No runtime framework/config or executable CAD/slicer checker supplied by these plans; no installs/config edits allowed. |
| Quick run command | Future C03 artifact-presence commands only; not geometry or human-authority proof. |
| Full suite command | Future file checks plus mandatory actual CAD/source/coupon/physical review; no invented FreeCAD/slicer CLI flags. |
| Estimated runtime / feedback latency | Unknown/unmeasured; no calibrated or measured seconds asserted. |

## Sampling Rate
- After each **approved task**, perform its authorized declaration/manual inspection; an eventual runnable binding requires its own approved scope. No commit sampling or Git.
- After approved wave 1/03-01, review actual candidate/revisions before independently approved wave 2/03-02; shared CAD is sequential.
- After approved wave 2, review current slice/cumulative waste and real physical evidence; missing proof blocks acceptance, not silently green handoff.
- All copied check text is dormant; no check/startup/measurement/model/tool acquisition occurs during this supplement.

## Per-Task Verification Map
| Task ID | Plan | Wave | Requirement | Threat Ref | Secure Behavior | Test Type | Automated Command | File Exists | Status |
|---------|------|------|-------------|------------|-----------------|-----------|-------------------|-------------|--------|
| 03-01-01 | 03-01 | 1 | IDN-01/02/03/04/08 scope | T-03-01/SC | Real new grant | Human + presence | C03-01-01 below | future permission evidence | pending |
| 03-01-02 | 03-01 | 1 | IDN-01/02/03/04/08 | T-03-02/03 | Actual envelope/service geometry | Manual CAD + presence | C03-01-02 below | future CAD/review | pending |
| 03-01-03 | 03-01 | 1 | IDN-01/02/03/04/08 | T-03-02/03/04 | Simultaneous fit/captive assembly | Manual CAD + presence | C03-01-03 below | future CAD/parts | pending |
| 03-02-01 | 03-02 | 2 | IDN-05/06/07 scope | T-03-05/SC | Independent grant | Human + presence | C03-02-01 below | future permission/evidence | pending |
| 03-02-02 | 03-02 | 2 | IDN-05/06/07 | T-03-05/06/07/08 | Actual coupons/slice/set/cost | Manual physical + presence | C03-02-02 below | future coupons/print record | pending |
| 03-02-03 | 03-02 | 2 | IDN-02/03/04/05/06/07/08 | T-03-04/05/06/07/08 | Actual fit/stability/heat | Manual physical + presence | C03-02-03 below | future prototype/revision | pending |
All six existing tasks/eight IDN IDs are mapped; actual messages, measurements and physical outcomes cannot be automated by document strings.

## Future artifact checks and mandatory manual proof
These commands mirror correctedPLANs and check existence only. Authority/geometry/fit/material/safety need actual human/source/physical evidence; none run now.
**C03-01-01 — Gate, 03-01.**
<automated>test -s .planning/phases/03-industrial-design/03-SPEC.md && test -s .planning/phases/03-industrial-design/03-CONTEXT.md</automated>
<fails_when>Only planning permission exists, the grant names another plan, its scope is insufficient, or prerequisite evidence is missing.</fails_when>
**C03-01-02 — Work 1, 03-01.**
<automated>test -s cad/industrial-design/Jalak-Gembul.FCStd && test -s .planning/phases/03-industrial-design/03-CHARACTER-ENVELOPE.md && test -s .planning/phases/03-industrial-design/03-CAD-REVIEW.md</automated>
<fails_when>Geometry is invalid, the path collides, actual dimensions are absent, representative dimensions are promoted to owner facts, height is double-counted, or copied geometry replaces the original concept.</fails_when>
**C03-01-03 — Work 2, 03-01.**
<automated>test -s cad/industrial-design/Jalak-Gembul.FCStd && test -s .planning/phases/03-industrial-design/03-PARTS.md && test -s .planning/phases/03-industrial-design/03-CAD-REVIEW.md</automated>
<fails_when>Required clear space or service access is lost, the mask pretends to enlarge the OLED, decoration is loose, dimensions/placements are fabricated, or pending physical/manufacturing checks are reported as accepted.</fails_when>
**C03-02-01 — Gate, 03-02.**
<automated>test -s .planning/phases/03-industrial-design/03-01-SUMMARY.md && test -s .planning/phases/03-industrial-design/03-CONTEXT.md</automated>
<fails_when>Only planning/03-01 permission exists, candidate/inventory evidence is absent, or any install/access/print/purchase/powered authorization is inferred rather than granted.</fails_when>
**C03-02-02 — Work 1, 03-02.**
<automated>test -s cad/industrial-design/Jalak-Gembul-Coupons.FCStd && test -s exports/industrial-design/Jalak-Gembul-Print-Set.3mf && test -s .planning/phases/03-industrial-design/03-PRINT-RECORD.md</automated>
<fails_when>Coupons are unprinted/unmeasured, per-side clearance is confused with total, roof/bridge/support removal is unvalidated, profile/wall revisions differ, PLA or costs are omitted/over cap, or assumed equipment/analytical grams replace actual evidence.</fails_when>
**C03-02-03 — Work 2, 03-02.**
<automated>test -s cad/industrial-design/Jalak-Gembul.FCStd && test -s exports/industrial-design/Jalak-Gembul-Print-Set.3mf && test -s .planning/phases/03-industrial-design/03-PRINT-RECORD.md && test -s .planning/phases/03-industrial-design/03-FIT-REVISION.md</automated>
<fails_when>Physical evidence or applicable grants/limits are missing, caps/clear-space/readability/retention/stability/heat criteria fail, a lean candidate loses payload, an obsolete slice is reused, or prototype checks are presented as child/toy certification.</fails_when>

## Wave 0 Requirements — Future Inputs, All Pending
- [ ] MAIN/owner: actual Phase 1 source evidence plus real NEW 03-01/03-02 grants before their dispatch; distinct installation/access/coupon-print/full-print/purchase/powered approval receipts precede each operation.
- [ ] Owner/authorized measurer: actual module/connector/control/tool/display envelopes and uncertainty; future `03-CHARACTER-ENVELOPE.md`/`03-CAD-REVIEW.md`, never representative drawings as owner facts.
- [ ] Authorized designer after 03-01 grant/tool access: `cad/industrial-design/Jalak-Gembul.FCStd`, `03-PARTS.md`, reviewed service/geometry record and `03-01-SUMMARY.md`; File Exists: no/future.
- [ ] Authorized designer/operator after 03-02 AND coupon/print access grants: `cad/industrial-design/Jalak-Gembul-Coupons.FCStd` and real measured closure/window/root/bridge coupons in `03-PRINT-RECORD.md`; no unprinted fixture counted.
- [ ] Authorized operator: actual machine/nozzle/spool/profile/quote and complete layer/support ledger for `exports/industrial-design/Jalak-Gembul-Print-Set.3mf`/`03-PRINT-RECORD.md`; every coupon/reprint/waste included; File Exists: no/future.
- [ ] Qualified adult operator after full-print/power grants: actual serviced/assembled prototype, safe authorized display/audio fixtures, limits and fit/face/tilt/heat results in `03-FIT-REVISION.md`/`03-02-SUMMARY.md`; File Exists: no/future.
- [ ] MAIN: approve artifact-check/manual-CAD/physical scopes; no artifact command validates geometry or creates installation authority. T-03-SC retains legitimacy gate.

## Manual-Only Verifications
| Behavior | Requirement / threat | Why Manual | Future instructions / status |
|----------|----------------------|------------|------------------------------|
| Real plan/operation permission and source access | All IDN / T-03-01, T-03-05, T-03-SC | Human authorization is not machine-created | MAIN checks real messages, prerequisites, scope/date/revocation before startup/operations; pending. |
| Original three forms/actual envelopes | IDN-01, IDN-02, IDN-03, IDN-08 / T-03-02, T-03-03 | Published examples cannot measure owner parts or establish original appeal | Authorized reviewer compares independent form/true OLED aperture and measured records; pending. |
| Service sweep and captive assembly | IDN-02, IDN-04 / T-03-03, T-03-04, T-03-07 | CAD/file presence cannot establish physical access/retention | Check actual largest serviced envelope through ≥75×60mm opening and ten proposed closure cycles after print approval; pending. |
| Actual printer/coupons/support/material ledger | IDN-05, IDN-06, IDN-07 / T-03-06, T-03-07 | Real fit/support removal/PLA are not analytical strings | Complete final-set PLA with its waste<=170g; development coupons/reprints separate cumulative ledger/all cash within500k; electronics excluded; pending. |
| Face/stability/thermal/acoustic evidence | IDN-03, IDN-06, IDN-08 / T-03-04, T-03-07 | Physical configuration/limits require qualified measurements | Approved fixtures: 0.5m face, all-direction 10° tilt, applicable heat limits and safe power; no child debugging/certification; pending. |
| Revision/lean preservation | IDN-02, IDN-05, IDN-07 / T-03-08 | Old slices or grams cannot validate changed geometry | Link changed CAD/profile to re-slice/measurements; retain 70×55×80 clear space and measured service/face/strength; pending. |

## Validation Sign-Off — Pending, Not Runtime Acceptance
- [ ] Real independent named grants/prerequisites and operation permissions checked before startup/actions.
- [ ] Wave 0 actual inputs/fixtures and authorized runnable bindings established; no invented tools or physical results.
- [ ] Task/wave feedback recorded only after approved work; every IDN/threat linked to current source/revision and required manual evidence.
- [ ] Complete support/raw-PLA/cumulative waste and aperture/fit/safety gates satisfied by actual evidence, not analytic estimates or document matches.
- [ ] Feedback duration measured only if authorized; no fabricated seconds, green result or child/toy certification.
- [ ] Retain `status: draft`, `nyquist_compliant: false`, `wave_0_complete: false` while runtime scaffolding/execution/proof are absent; MAIN owns later status changes.
**Approval:** pending. No existing SPEC/CONTEXT/PLAN/root/config changes, scripts/tests, CAD/slicing/printing or live proof performed here.
