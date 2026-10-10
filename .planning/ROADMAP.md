# Roadmap: Si Jalak Bedas v0.1

Planning baseline2026-10-09, delegated YOLO decisions. Eight phases/eleven plans. Phase1/01-01 limited documentary execution started after authentic no-Git grant; Task1complete, Task2blockedbeforework,0plans/phasescomplete. Other phases not started. Every plan has a blocking scoped authorization checkpoint; planning-only permission cannot approve it. No dates invented.

## Workstreams
- **Offline software (AUTH-U-03, exploration stream, NOT a phase)**: phases/00-software-offline — firmware logic + backend core executable with no hardware/credentials/field. Does not complete any phase/requirement or gate. Ledger: `.planning/DEPENDENCY-CLASSIFICATION.md`; evidence: `.planning/EVIDENCE/OFFLINE-EVIDENCE.md`.
- Product/field evidence: phases1,2,7,8; field access and provider eligibility remain external gates.
- Voice/firmware: phases1,4,5,6; first milestone is internal adult D3 demo, not child AI rollout.
- **Industrial Design, 3D Printable Enclosure & Character Design**: phase3, integrated/fit checks in6 and operator feedback in7/8. Scope now is parametric envelope/character/assembly/print strategy, not detailed PCB mounting/wiring.

## Phases
- [ ] **Phase 1: Baseline Contracts and Safety** — inventory, API contract, budget/provenance and release boundaries.
- [ ] **Phase 2: TBM Discovery and Curation** — authorized observation and small real catalog; can run in parallel after1 without blocking technical demo.
- [ ] **Phase 3: Industrial Design and Printable Enclosure** — original hybrid, usable envelope, parametric CAD/printing preparation after authorization.
- [ ] **Phase 4: VPS Speech Gateway and AI Guardrails** — isolated bounded adult STT/chat/TTS tracer and evals.
- [ ] **Phase 5: ESP32 Voice and Expression** — device E2E tracer plus isolated child-local profile.
- [ ] **Phase 6: Integrated Prototype and Adult Demo** — physical/acoustic/power/latency readiness and repeatable real voice demonstration.
- [ ] **Phase 7: Literacy Pilot and Evaluation** — consented local-mode pilot, matched comparison, no child Gemini traffic.
- [ ] **Phase 8: Iteration and Handover** — evidence-based revisions and operator package.

## Phase Details

### Phase 1: Baseline Contracts and Safety
**Goal**: maintainers can see actual inventory/contracts and which operations are permitted before building anything.
**Depends on**: Nothing.
**Requirements**: [BAS-01, BAS-02, BAS-03]
**Success Criteria**:
1. Actual labels/dimensions and sanitized STT/chat/TTS contract evidence replace or flag packaging/audio assumptions.
2. Gemini eligibility, power, budget, data and external permissions have explicit gates; no measured result is invented.
3. Interface revision and environment package/model pins are recorded before integration.
**Plans**: 1 plan.
Plans:
- [ ] 01-01: Inventory, contract and safety freeze.

### Phase 2: TBM Discovery and Curation
**Goal**: owner/operator obtains permissioned evidence about reading starts and a small usable book catalog.
**Depends on**: Phase 1; independent of phase4–6 demo completion.
**Requirements**: [RSR-01, RSR-02, RSR-03]
**Success Criteria**:
1. Partner/access/consent exist before observation; baseline and interviews are anonymized.
2. Cohort, reading-start proxy and6–12 local titles are based on site evidence or revised transparently.
3. Robot/no-robot concept comparison is prepared without claiming impact from preference responses.
**Plans**: 1 plan.
Plans:
- [ ] 02-01: Partner gate, baseline and catalog.

### Phase 3: Industrial Design and Printable Enclosure
**Goal**: character/casing can be developed and checked against room, printability, material and assembly targets without overdesigning internals.
**Depends on**: Phase 1; can run alongside2/4.
**Requirements**: [IDN-01, IDN-02, IDN-03, IDN-04, IDN-05, IDN-06, IDN-07, IDN-08]
**Success Criteria**:
1. Rounded monster/blob/hybrid comparison leads to original Jalak Gembul with documented parameters and no copied/AI-generated art.
2. Measured packaging/visible OLED area/service paths constrain the CAD; mounting/wiring is not invented in planning.
3. Authorized future slice totals all PLA/waste against170g cap; roof/overhang/fit/stability evidence exists before print/child use.
**Plans**: 2 plans.
Plans:
- [ ] 03-01: Character, envelope and parametric CAD boundaries.
- [ ] 03-02: Coupon, slicing, assembly and fit revision.

### Phase 4: VPS Speech Gateway and AI Guardrails
**Goal**: a verified isolated adult R&D voice turn can be processed and bounded without altering unrelated services or exposing secrets.
**Depends on**: Phase 1; uses clearly labelled demo catalog if2 not finished.
**Requirements**: [API-01, API-02, API-03, API-04, AI-01, AI-02, AI-03, AI-04, AI-05, AI-06]
**Success Criteria**:
1. Real STT→chat→TTS adult tracer is possible only after explicit provider/access authorization; mocks are labelled.
2. Device ownership, idempotency, resets, quotas and content validation prevent stale/cross-device responses and unbounded expense.
3. Adult-only eligibility and negative eval evidence are recorded; no child-directed Gemini client is deployed.
**Plans**: 2 plans.
Plans:
- [ ] 04-01: Isolated protocol/provider tracer and device authorization.
- [ ] 04-02: Privacy, content, faults, cost and eval verification.

### Phase 5: ESP32 Voice and Expression
**Goal**: user initiates a bounded turn on the robot, understands its state, hears the response and can stop it.
**Depends on**: Phase 1 and Phase 4.
**Requirements**: [FW-01, FW-02, FW-03, FW-04, FW-05]
**Success Criteria**:
1. Input WAV16k/output WAV24k and actual mic/amp/display are verified without assuming board GPIO/controller variants.
2. Device API client and OLED/control state machine form a real tracer; no mic capture during playback or after stop.
3. Child-local build/profile has no enabled live-Gemini route, voice capture or user history; curated content works offline.
**Plans**: 2 plans.
Plans:
- [ ] 05-01: Adult voice/OLED/control tracer.
- [ ] 05-02: Isolated local profile, content and reset/fault hardening.

### Phase 6: Integrated Prototype and Adult Demo
**Goal**: the assembled robot demonstrates actual AI voice safely and repeatably in a restricted consenting-adult setting.
**Depends on**: Phase 3 and Phase 5; no phase2 dependency for internal technical demonstration.
**Requirements**: [INT-01, INT-02, INT-03]
**Success Criteria**:
1. Robot microphone and speaker participate in real AI turns; partial tests/recorded fallback aren't counted as integration success.
2. Power, temperature, stop, case closure, acoustics and state recovery pass documented checks.
3. Ten-cycle smoke test, proposed60-minute powered run and latency/fault measurements are recorded; no invented pass.
**Plans**: 1 plan.
Plans:
- [ ] 06-01: Physical integration, safety and controlled adult demo.

### Phase 7: Literacy Pilot and Evaluation
**Goal**: learn whether a local curated character intervention helps actual reading starts without provider/privacy violations.
**Depends on**: Phase 2 and Phase 6; additional child-profile/provider-eligibility gate mandatory.
**Requirements**: [PIL-01, PIL-02, PIL-03]
**Success Criteria**:
1. Actual site/consent/guardian+child participation and isolated local build are approved; no Gemini traffic/voice capture from child unit.
2. Hooks/choices reference available books and transfer attention, not a points loop or endless chat.
3. Matched reading-start proxy, help/distraction/operator burden and denominators are reported with limitations.
**Plans**: 1 plan.
Plans:
- [ ] 07-01: Local-mode pilot and matched feasibility evaluation.

### Phase 8: Iteration and Handover
**Goal**: owner/operator can maintain an evidence-labelled release and decide continue/revise/stop.
**Depends on**: Phase 7.
**Requirements**: [OPS-01, OPS-02]
**Success Criteria**:
1. Content/profile/stop/power/cleaning/storage/recovery responsibilities are documented without credentials.
2. Decision reflects evidence and unresolved risks, not extrapolated comprehension/habit claims.
**Plans**: 1 plan.
Plans:
- [ ] 08-01: Evidence review, bounded revisions and operator handover.

## Progress
| Phase | Plans Complete | Status | Completed |
|-------|----------------|--------|-----------|
| 1. Baseline | 0/1 | Documentary packet done; phase goal NOT_VERIFIED, G1 BLOCKED | - |
| 2. Discovery | 0/1 | Not started | - |
| 3. Industrial design | 0/2 | Not started | - |
| 4. VPS/AI | 0/2 | Not started | - |
| 5. Firmware | 0/2 | Not started | - |
| 6. Integration/demo | 0/1 | Not started | - |
| 7. Pilot | 0/1 | Not started | - |
| 8. Handover | 0/1 | Not started | - |

Execution is a dependency graph:1→{2,3,4};4→5;{3,5}→6;{2,6}→7→8. Plans ready does not mean phases complete. No stock auto-chain/execute workflow may run under current permission.
