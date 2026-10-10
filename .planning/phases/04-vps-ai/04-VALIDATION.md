---
phase: '04'
slug: vps-ai
status: draft
nyquist_compliant: false
wave_0_complete: false
created: '2026-10-09'
---

# Phase 04 — Future Validation Strategy

Planning supplement only. No tests/fixtures/code were created or run; no permission, provider access, green result or runtime compliance is established.
The six existing tasks, ten requirement IDs and all phase threat references are mapped below; planning coverage review is separate from runtime/Nyquist status.
Sources: `.planning/phases/04-vps-ai/04-SPEC.md`, `.planning/phases/04-vps-ai/04-CONTEXT.md`, `.planning/phases/04-vps-ai/04-01-PLAN.md`, `.planning/phases/04-vps-ai/04-02-PLAN.md`, `.planning/phases/04-vps-ai/04-RESEARCH.md`, `.planning/research/AI-RESEARCH.md`, `.planning/VERIFICATION-STRATEGY.md`, `.opencode/gsd-core/templates/VALIDATION.md`.

## Test infrastructure — future only
| Property | Contract |
|----------|----------|
| Framework | Planned Python3.12 unittest; FastAPI/provider dependencies and actual pins/install state unverified |
| Config / fixtures | No new test config; fixtures belong inside the planned test files and `backend/evals/cases.json` |
| Quick command | `python3.12 -m unittest discover -s backend/tests -p test_adult_tracer.py -v` — future labelled mock mode only |
| Full suite | Applicable non-live commands in the task map; no new aggregate runner or command invented |
| File/runtime evidence | Every verification target below: No, future-only; runtime and feedback latency unknown/not measured |
| Setup authority | Named-plan G0 first; separate install approval and actual package legitimacy/compatibility/pin evidence before dependencies |

## Sampling rate — only after approval
- No workflow/executor or Wave0 creation starts to obtain approval; MAIN first records a new explicit decision for each named plan.
- After each approved task, run that task's mapped check within its separately granted scope; a receipt check never substitutes for human authorization.
- After wave1, rerun04-01 non-live tracer/protocol checks; after wave2, rerun all applicable non-live04-01/04-02 checks from the map.
- No Git/commit trigger. Live commands are excluded from automatic sampling and require actual adult eligibility/provider/data/billing authorization.
- Before future phase runtime sign-off, review all gate receipts, non-live results, separately approved real evidence, human rubrics and unresolved failures.
- Feedback latency: unknown;30s job deadline is a selected bound, not measured test runtime. Each live eval invocation≤one bounded case; aggregate≥30 respects5/minute and4-turn/180s limits.

## Per-task verification map — commands copied from existing plans, not executed
| Task ID | Plan | Wave | Requirement IDs | Threat Ref | Secure behavior / failure meaning | Test type | Exact future automated command | File Exists | Status |
|---------|------|------|-----------------|------------|-----------------------------------|-----------|--------------------------------|-------------|--------|
| 04-01-01 | 04-01 | 1 | API-01, API-02, API-03, API-04, AI-01, AI-02, AI-03 | T-04-01, T-04-SC | New named approval before workflow; fail if absent/inherited or external scope missing; existence cannot approve | Receipt existence + manual gate | `test -s .planning/phases/04-vps-ai/04-01-AUTHORIZATION.md` | No — future receipt | pending |
| 04-01-02 | 04-01 | 1 | API-01, API-02, API-03, API-04, AI-01, AI-02, AI-03 | T-04-01, T-04-02, T-04-03, T-04-04 | Owned canonical real backend turn; fail on missing fixture/authorization/conformance, unsafe call, stale publication or mock-as-live | E2E mock; separately gated real E2E | `python3.12 -m unittest discover -s backend/tests -p test_adult_tracer.py -v`<br>`SIJALAK_VERIFY_MODE=live python3.12 -m unittest discover -s backend/tests -p test_adult_tracer.py -v` | No — future test | pending |
| 04-01-03 | 04-01 | 1 | API-02, API-03, API-04, AI-01, AI-02, AI-03 | T-04-01, T-04-02, T-04-03, T-04-04 | Auth/ownership/idempotency/reset/rate/cost denial; fail on unauthorized or stale access, extra inference, budget leak or contract mismatch | Protocol/adversarial | `python3.12 -m unittest discover -s backend/tests -p test_device_protocol.py -v` | No — future test | pending |
| 04-02-01 | 04-02 | 2 | AI-04, AI-05, AI-06, API-03, API-04 | T-04-09, T-04-SC | New04-02 approval, not inherited04-01; fail absent scope; provider/account/data authority needs human review | Receipt existence + manual gate | `test -s .planning/phases/04-vps-ai/04-02-AUTHORIZATION.md` | No — future receipt | pending |
| 04-02-02 | 04-02 | 2 | AI-04, AI-05, API-03, API-04 | T-04-05, T-04-06, T-04-07, T-04-09 | Cancel/expiry/redaction and honest fallback; fail on accessible late content, raw/secret logging or fallback counted real_ai; remote deletion not promised | Lifecycle/fault/privacy E2E | `python3.12 -m unittest discover -s backend/tests -p test_privacy_faults.py -v` | No — future test | pending |
| 04-02-03 | 04-02 | 2 | AI-04, AI-05, AI-06, API-04 | T-04-05, T-04-07, T-04-08 | Safe actions/catalog/cost and honest denominators; fail unauthorized eval, mismatched prices, negative violation, missing sample/rubric, false PASS or misclassified fallback/device metric | Policy/cost fixtures; approved adult eval | `python3.12 -m unittest discover -s backend/tests -p test_cost_policy.py -v`<br>`python3.12 -m unittest discover -s backend/tests -p test_ai_evals.py -v`<br>`SIJALAK_VERIFY_MODE=live python3.12 -m unittest discover -s backend/tests -p test_ai_evals.py -v` | No — future tests/cases | pending |

## Wave 0 requirements — future preparation within each approved plan
- [ ] 04-01 G0 receipt supplied/reviewed by MAIN before any test/dependency preparation;04-02 needs its own new G0 and separate affected scopes.
- [ ] `backend/tests/test_adult_tracer.py` — injected async role/protocol mocks, canonical WAV PCM/silence/malformed/oversize fixtures, approved real-mode denial conditions.
- [ ] `backend/tests/test_device_protocol.py` — two-device auth/ownership, duplicate races/digests, reset/generation, expiry/restart, rate/session caps and unknown-price denial.
- [ ] `backend/tests/test_privacy_faults.py` — deterministic clocks/slow completions, stage cancel,60s/10min upper expiries, redaction markers and truthful local/error outcomes.
- [ ] `backend/tests/test_cost_policy.py` — synthetic rate/FX/unit tables, atomic reservations/duplicates/uncertain billing; unconfigured/exhausted budget denies rather than assumes free.
- [ ] `backend/evals/cases.json` and `backend/tests/test_ai_evals.py` — synthetic normal/adversarial/PCM/unsupported-model fixtures and expected actions; no child/private corpus.
- [ ] Embed fixtures in those already-planned files; do not invent additional harness paths, private recordings, child scripts, installed SDKs or passing test results.
- [ ] Missing/empty/skipped fixture sets are not green; dependencies require separate actual install approval, audit and pins, never implicit downloads or global credentials.
- [ ] Wave0 is preparation inside authorized task order, not a preauthorization phase;04-01 full backend tracer must precede expansion and04-02.

## Manual-only verifications — never replaced by grep/file existence
| Behavior | Requirement / threat | Why manual | Future evidence required |
|----------|----------------------|------------|--------------------------|
| Named approval and each external/install/billing scope | All mapped IDs; T-04-SC | Only owner can grant scope; file presence cannot | MAIN-reviewed new04-01/04-02 receipts; revoked/missing scope halts affected operation |
| Actual offering/account/18+ restricted R&D client and models/locale | AI-01, AI-02; T-04-01, T-04-03, T-04-09 | Indexed documentation cannot validate owner account or eligibility | Approved actual contract/role/locale evidence before live calls; no child/proxy/guardian/paid bypass |
| Current exact-model prices/FX/allowance and provider retention | API-04, AI-04, AI-05; T-04-08, T-04-09 | Billing authority/terms not established by mock rates | Approved snapshots/data settings; zero-retention/remote-delete not inferred; unknown pricing blocks calls |
| Adult consent, semantic rubrics and actual audible device behavior | AI-03, AI-04, AI-06 | Human judgment/physical capture and speaker cannot be proved by backend grep | Synthetic/consenting-adult cases and human scoring; robot capture/p95/safety remains05/06 evidence, not backend substitution |
| Safe powered device bench if involved | API-01, AI-02; prior G1/G3 | Hardware/supply conditions are unknown | Separate hardware permission and actual power/stop checks; no hardware testing or safety claim here |

## Failure and evidence discipline
- Any unauthorized child/provider call, stale/cross-session access, invalid/harmful allowed action or content/secret leak blocks future acceptance; zero chosen-fixture failures is not universal safety.
- Mock/local fallback is disclosed and excluded from AI-success counts; aggregate≥30 and target scores remain incomplete until real approved evidence exists; no fabricated green rows.
- Only allowed metadata/case IDs and human rubrics are retained; reset is application cleanup, not provider deletion. No actual private content is created/collected now.

## Validation sign-off — runtime pending
- [ ] All mapped future checks actually exist and ran within authorization; no empty/skipped suite accepted.
- [ ] Wave0 preparation, task/wave sampling and manual gates evidenced; actual feedback latency recorded, not guessed.
- [ ] Real versus mock/fallback evidence and failures reviewed; device-dependent measures remain correctly pending.
- [ ] Runtime status/flags may change only in a separately authorized later validation task; this supplement leaves draft/false/false unchanged.
Approval: pending. Planning-only task/REQ/threat/command coverage can be reviewed separately; it does not authorize execution or establish Nyquist compliance.
