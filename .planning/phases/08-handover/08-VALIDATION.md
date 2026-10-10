---
phase: '08'
slug: handover
status: draft
nyquist_compliant: false
wave_0_complete: false
created: 2026-10-09
runtime_executed: false
tests_executed: false
evidence_collected: false
fieldwork_executed: false
---

# Phase 08 — Draft Validation Strategy

Planning supplement only; no document command, release inspection, receipt, owner decision, handover or rollout was performed. Existing/root/config artifacts remain unchanged.
Fresh named08-01 approval must precede workflow/executor startup; its first checkpoint cannot be approved from headers/file markers. Test-command, device/server/field/contact/billing/print/install/Git authority remains separate; documentation permission is no rollout permission.

## Test infrastructure — prospective document checks only
| Property | Current / future contract |
|---|---|
| Framework / config | No runtime test suite or config declared; existing PLAN contains future local `test`/`grep` document checks plus human source/tabletop review, no install added. |
| Quick check | Exact task-specific document command below, only after actual named authority, future files and separately allowed command execution. |
| Full suite | No independent suite declared; after approved wave1, review all three PLAN checks and human contracts, not a newly invented runner. |
| Runtime / maximum feedback latency | Unknown, not measured; the PLAN's token estimate is not runtime or feedback evidence. |
| Current test / evidence availability | No — future authorization, guides/reports/verification/summary; actual prior manifest and acknowledgements not supplied. |

## Per-task verification map — exact future PLAN commands
| Task ID | Plan | Wave | Requirement | Threat Ref | Secure Behavior | Test Type | Automated Command | File Exists | Status |
|---|---|---|---|---|---|---|---|---|---|
| 08-01-01 | 01 | 1 | OPS-01, OPS-02 authority/input gate | T-08-03, T-08-06, T-08-SC | Actual named scope and supplied prior manifest before startup; marker never authorizes | Document structure + human decision | `test -s .planning/phases/08-handover/08-AUTHORIZATION.md && grep -Fq "Approval: explicit 08-01 documentation-only" .planning/phases/08-handover/08-AUTHORIZATION.md` | No — future `08-AUTHORIZATION.md` | pending |
| 08-01-02 | 01 | 1 | OPS-01 | T-08-01, T-08-03, T-08-04, T-08-05, T-08-06 | Source-backed profile/content/power lifecycle, STOP/escalation and accountable roles | Document structure + manual tabletop/source review | `test -s docs/handover/operator-guide.md && test -s docs/handover/maintainer-guide.md && test -s docs/handover/incident-runbook.md && grep -Fq "OPERATOR-PATH-01" docs/handover/operator-guide.md && grep -Fq "OPERATOR-PATH-01" docs/handover/maintainer-guide.md && grep -Fq "OPERATOR-PATH-01" docs/handover/incident-runbook.md` | No — all three future guides | pending |
| 08-01-03 | 01 | 1 | OPS-01, OPS-02 | T-08-02, T-08-03, T-08-04, T-08-06, T-08-SC | Actual evidence/limits/costs/receipts and owner-reviewed recommendation, never automatic rollout | Document structure + manual evidence/acknowledgement review | `test -s docs/handover/evidence-review.md && test -s docs/handover/revision-register.md && test -s .planning/phases/08-handover/08-VERIFICATION.md && grep -Fq "OPS-01" .planning/phases/08-handover/08-VERIFICATION.md && grep -Fq "OPS-02" .planning/phases/08-handover/08-VERIFICATION.md` | No — future review/register/verification | pending |
Commands are verbatim, checkout-root-relative future text only. Structural presence can match a misleading header: it cannot establish permission, correct content, actual physical/field evidence or received handover.

## Future Wave 0 manifest/checklist/fixture preparation
Only inside the approved existing documentation scope after G0 and relevant permissions; no pre-checkpoint bootstrap, extra plan/wave or executable fixture/test suite is introduced.
- [ ] Future `.planning/phases/08-handover/08-AUTHORIZATION.md`: actual scoped statement/date and MAIN-supplied sanitized Phase7 evidence paths, including truthful blocked/stopped report; missing sources halt dependent drafting.
- [ ] Future `docs/handover/operator-guide.md`, `maintainer-guide.md`, `incident-runbook.md`: source-backed OPERATOR-PATH-01 checklist and explicit withdrawn/unknown-source variant, each with prerequisite/observation/failure/stop/role.
- [ ] Document-level hypothetical fixture scenarios: stale profile/content, unavailable book/maintainer, missing acknowledgement, unsafe rollback, privacy/secret leak and STOP/power/heat incident; label hypothetical, not actual release defects or tested outcomes.
- [ ] Future `docs/handover/evidence-review.md`: every canonical requirement's prior source/revision/scope/status and proposed-versus-measured distinction; no rerun of another phase or manufactured field result.
- [ ] Future `docs/handover/revision-register.md`: evidence-linked permitted wording corrections, required recheck and separate operation gate; firmware/CAD/provider/deploy/print/field changes remain recommendations, not actions here.
- [ ] Care/power checklist consumes actual manufacturing/service/thermal/protection evidence; no invented wiring/limits, charging or450mAh runtime guarantee, no CAD/printing or physical activation to fill gaps.
- [ ] Prior benchmark/proxy ledger preserves proposed≥30 adult attempts/10 cycles/60 minutes/p95≤10s and exploratory10–20 field opportunities/condition/≥30s content proxy as targets unless actual accepted sources exist; no efficacy conclusion from the numbers.
- [ ] Future `08-VERIFICATION.md` and `08-01-SUMMARY.md`: factual review/receipt/maintainer status or pending/blocked, without owner decision or rollout inferred from a marker.
No dependency installs or executable test files are planned here. All future checklist/fixture/evidence preparation is pending; Wave0 is not complete.

## Sampling after approved tasks/waves — never commits
- Before startup/Task1: MAIN verifies actual fresh owner statement/scope and manifest independently; the future Task1 command cannot be used to grant dispatch authority.
- After each approved task, when named future files and command-execution permission exist: sample that task's exact document command and human source/failure contract; missing files/permissions keep it pending/blocked.
- After local wave1: review all three command outcomes, actual evidence links and owner/operator/maintainer review; no new full-suite command, automatic rollout or next workflow.
- Prior to any accepted handover: reconcile required real acknowledgements and unresolved safety/eligibility/budget/compatibility gates; draft documents alone are not acceptance.
- No commit/Git/tag/hook sampling. Runtime and feedback duration are unknown; no measured latency, timing budget or completion is invented.

## Manual-only verification and evidence limits
| Behavior | Requirement / threats | Why manual / future evidence required |
|---|---|---|
| Named scope and actual predecessor manifest | Both OPS; T-08-03, T-08-06, T-08-SC | MAIN checks the real new owner decision and supplied evidence before startup; stopped/blocked Phase7 evidence is legitimate, not successful pilot proof. |
| Profile/content/rollback and operator lifecycle | OPS-01; T-08-01, T-08-05, T-08-06 | Tabletop source review only, no device/server activation; signed/deployment child-local stays no-mic/no-Gemini/cloud-egress, no user/parent/proxy/paid bypass. |
| Care/power/manufacturing and incident duties | OPS-01; T-08-02, T-08-05 | Actual approved rails/thermal/fit/service sources and accountable escalation required; headers cannot verify CAD/print/power; unknown/unsafe restart/charging stays blocked. |
| Evidence interpretation and actual costs | OPS-02; T-08-02, T-08-06 | Check real Phase7 denominators/help/harms/limits and prior criteria; rates/billed units/FX/hosting/quotes or unknown, no invented benefit/battery/runtime/cost or retrospective favourable target. |
| Protected participant/consent and secret boundaries | Both OPS; T-08-04 | Consent/notes/identity records private outside project; consume only authorized disclosure-reviewed participant aggregates, no private content/paths/credentials in guides or incident package. |
| Owner decision, operator receipt, maintainer acknowledgement | OPS-01/02; T-08-03, T-08-06 | Actual separately supplied review/receipt required or pending; continue/revise/stop is future accountable review, never automatic release/deploy/spend/field authorization. |
This phase consumes prior physical/field evidence, never claims its document checks tested hardware, renewed provider eligibility or proved literacy efficacy. No actual continue/revise/stop conclusion exists now.

## Draft sign-off — all pending, not run or done
- [ ] Every existing task/OPS requirement/threat has a future command/manual route; command semantics and actual authority are checked separately after permission.
- [ ] Actual input manifest and future guides/reports/checklists exist; no Wave0 activity or document tests are complete now.
- [ ] Source/revision/status/privacy/cost checks and real receipt/owner/operator/maintainer review are recorded, not inferred from text.
- [ ] Remaining gates withhold operational handover and rollout; root/config/state/Git/global changes remain outside scope.
**Approval:** pending. **Runtime/feedback:** unknown. `status: draft`, `nyquist_compliant: false`, `wave_0_complete: false`; all runtime/test/evidence/field flags remain false.
