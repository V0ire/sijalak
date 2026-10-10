# Verification strategy — plans vs execution evidence

2026-10-09. This file specifies future verification, not passing results. `PLANNING-VERIFICATION.md` will report checks performed on planning artifacts only. Never mark a requirement implemented from a parser/review pass.

## Evidence / gates
| Gate | Evidence required | Currently |
|------|-------------------|-----------|
| G0 implementation authority | Explicit new owner permission for named plan; further install/Git/field/server/billing permissions remain separate | Not granted |
| G1 inventory/contract | Actual variant/dimension data, sanitized API/schema docs, model/version/rate/eligibility/account review | Owner reports + public research only |
| G2 enclosure/print | Approved CAD, aggregate slicer mass including waste, overhang/roof/access review, coupons and measured fit | No CAD/slice/print |
| G3 technical safety | Verified power/protection/thermal/closure/stability/volume/stop checks | No electrical tests |
| G4 adult AI | Authorized eligible restricted client, real robot audio turn, eval/fault/cost/session evidence | No model/API calls |
| G5 child field | Actual partner/consent, isolated local build with no Gemini/voice egress, real catalog, baseline/comparator | No fieldwork |

## Planning checks that may run now
- Native OpenGSD planner/checker used directly with no CLI/init/hooks/Git; all planning choices auto-selected with rationale.
- Requirement ID uniqueness and exact one-phase coverage; plan requirements match roadmap; prerequisite DAG acyclic and valid.
- PLAN frontmatter/task structure via source-proven local parser functions, without stock CLI startup/config/state writes.
- Each future plan autonomous:false, first checkpoint:decision gate=blocking-human, no auto_select, explicit no implementation authority now.
- Roadmap/STATE distinguish plans-created from execution progress;0 completed requirements/plans/phases.
- Config mode:yolo only for planning, auto_advance:false, _auto_chain_active:false, commit_docs:false, create_tag:false; global config/plugins/MCP not enabled/modified.
- Source/provenance distinction, primary Gemini terms restriction, PLA consumption vs assembled mass, separate profiles, no actual source/CAD/STL/G-code artifacts.

## Future unit / contract checks — only after permission
1. PCM fixtures: silence/ramp/clipping, raw mic-slot conversion, WAV metadata/rate/endian, size/duration caps and malformed headers.
2. API fixtures: ownership/auth401/403, duplicate turn idempotency, stale generation, oversize/429, invalid provider output, timeout/cancel/TTS failure, budget unconfigured/exhausted. Synthetic data and stubbed adapters must be labelled.
3. Reply schema/persona/known-book allowlist: out-of-context, harmful request, prompt injection, absent catalog, long reply, invalid action/bookID. Prompt alone is not a passing guardrail.
4. Firmware state machine: no mic capture on idle/child/speaking/stop, repeatable start/release/stop, disabled-network local profile, controlledreset and heap/buffer bounds.
5. Pricing estimation: snapshot rates/currency/usage, admission caps, no duplicate paid jobs, metadata redaction. Monitoring is not a guaranteed external billing hard cap.

## Adult AI evaluation proposals
- Start with>=30 consenting-adult/synthetic voice turns across distinct intents plus negative fixtures; this is a small engineering set, not statistically representative.
- Score transcript intent fidelity (not inferred emotion), relevance, shortness, grounded books/action, safe refusal, profile/session isolation and playback intelligibility using recorded rubric decisions.
- Target>=90% correct intent for clear supported-language inputs,<=10s p95 end-utterance to first audible response under declared network,>=95% successful eligible technical turns. Proposed goals only; disclose CI/sample limitations and failures.
- Negative gates:0 cross-session leakage/stale audio in controlledcases;0 unauthorized child/cloud calls in build/egress tests;0 harmful or invented-action outputs allowed by controller in chosen fixture set. A0-failure fixture result is not a universal safety guarantee.
- Test full-path robot mic/speaker and case effects; PC-only audio is partial.10-cycle smoke test then60-minute external-power run; no child participant used for debugging.

## Mechanical / print evidence proposals
- Inventory measured modules, uninterrupted cavity and aperture/service paths checked in CAD; dimensions/clearance may change.
- Slicer totals all printed parts+support+brim+purge <=170gPLA goal. Net printed mass and assembled mass recorded separately; analytical shell estimate is not slicer proof.
- Confirm AD5M/nozzle/material profile and ventilation; wall/perimeter mapping, top closure/bridge checks, no hidden trapped supports.
- Calibrate0.25–0.40mm per-side fit allowance with coupons; fit largest servicedassembly, control reach, grille/mic opening, removable caps and repeated closure without brittle loose parts.
- Proposed10° assembled tilt checks all directions, no sharp edges/detachable horns, support removal practical; COM and physical retention unmeasured now. Not toy certification.
- Housing/battery limits conservative: propose external surface<=40°C and no charging/field-battery path until its manufacturer/protection limits are known. Stop on swelling, unusual heat/smell/instability; operators follow approved procedure, not firmware-only monitoring.

## Field discovery / pilot proposals
- Follow `DISCOVERY.md`; collect observations without identities/voice/photo. Partner/guardian/participant information and consent before participation, refusal with no loss of TBM access.
- Cohort10–14 is provisional. Suggest10–20 eligible opportunities per condition over at least2 periods for feasibility, not a powered efficacy study; adapt to attendance and approval.
- Reading-start proxy: voluntary content engagement with book for>=30s or reading with assistance, defined jointly; not merely touching/opening book. Record help and purpose of visit; do not diagnose comprehension.
- Same books/cues/attention options for robot and no-robot card/relawan comparison, counterbalanced blocks where feasible; report invitation/exposure/opt-in denominators, not all baseline vs selected robot users.
- Report transition proportion, time to book, help, distraction/queue/operator time, refusals and adverse issues; repeat exposure if possible, but no hidden individual tracking.
- Set an effect target with partner **before** interpreting pilot; do not promise uplift from current plan. Revise/no-robot outcome is legitimate.

## Reporting / exit
All future evidence goes to phase VERIFICATION/summary artifacts after authorized execution. Unresolved compliance, budget, safety, fit or source gaps block deployment, not completion of provisional planning. Final planning report names these gaps and stops before execution.
