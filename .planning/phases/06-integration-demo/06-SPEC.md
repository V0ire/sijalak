# Phase 06: Integrated Prototype and Adult Demo — Specification

**Created:** 2026-10-09. **Status:** planning/readiness candidate only; execution not started, no acceptance result.
**Requirements:** INT-01, INT-02, INT-03. **Phase dependencies:** accepted Phase 3 and Phase 5, including their Phase 1/4 prerequisites; Phase 2 is not required for this internal technical demo.
**Authority now:** write only `06-SPEC.md`, `06-CONTEXT.md`, `06-01-PLAN.md` here. Every other named artifact is a future output. No executor/workflow starts without fresh authorization for THIS `06-01`.
**Provenance:** U = owner report; D = recorded documentation; A = planning selection/target; V = verification pending. No interview or ambiguity score is fabricated.

## Goal
The assembled robot demonstrates actual AI voice safely and repeatably in a restricted consenting-adult setting, as required by `.planning/ROADMAP.md` Phase 6.

## Current → target → acceptance
| ID | Current | Target after separate implementation and external approvals | Acceptance — all pending |
|---|---|---|---|
| INT-01 | Modules and VPS/API STT/chat/TTS are reported; “Gemini, presumably” is unverified. No firmware/server/CAD or real-turn result is established by current planning. | Actual robot microphone → verified VPS/API → real STT → chat → TTS → actual robot speaker, on an eligible restricted internal 18+ client. | [ ] AC-01/02: consenting-adult device turn and role/codec/profile/network evidence; no operational PC/phone companion; PC-only, mock and recorded fallback cannot pass. |
| INT-02 | Case envelopes, power, temperature, fit, acoustics and controls are assumptions without physical evidence. | Measured actual assembly, service/control access, verified regulated external power and closed-case acoustic/STOP checks before demonstration. | [ ] AC-03/04: actual-module rail and fit checks, approved thermal limits, reachable PTT/STOP and no playback capture; no packing or battery-safety inference. |
| INT-03 | No smoke, endurance, latency, fault, quota, cost or purge measurements exist. | Revision-linked repeatability/performance/fault evidence and an honest readiness verdict. | [ ] AC-05/06/07: proposed ten-cycle smoke, 60-minute powered run and ≥30 eligible adult turns; failures, denominators, latency and spend reported, not invented. |

## Boundaries
- In scope: integrate accepted enclosure/device/gateway outputs; an actual adult D3 tracer; physical/rail/acoustic/control checks; smoke, endurance, measured performance, cancellation/privacy/fault evidence and a bounded operator demonstration procedure.
- Out of scope: new CAD/PCB mounts/wiring layout, firmware/backend feature development, provider substitution, TBM/event/child deployment, field recruitment, literacy-impact claims or toy certification. Regressions return to the owning Phase 3/4/5 plan under its own approval.
- Exactly one plan, `06-01`, local wave 1 with `depends_on: []`; this does not waive the external Phase 3+5 acceptance gates. Its dormant checkpoint precedes the first work task, the real-device tracer.
- Physical printing/commissioned printing, printer access, supplies/purchases, participant consent, device/build/flash, server/API access, model use, billing, installs and Git each need separate authorization; approving implementation grants none automatically.

## Constraints
- Provider gate: recorded public Gemini Developer terms require users 18+ and prohibit clients child-directed or likely accessed by under-18s. Verify the actual offering/account/contract and restricted deployment before calls; parent consent, adult relay, proxy/VPS or paid tier is not a bypass. No child debugging, child speech, field/event API input or Gemini-generated child scripts.
- Adult-lab and child-local remain distinct verified deployment/profile permissions, not an age toggle. Use synthetic material or consenting 18+ internal R&D speakers only; no personal/sensitive content, raw-content logs or participant identities in evidence.
- Reported modules are ESP32-S3 N16R8, OLED1.3-inch128×64, INMP441, MAX98357A/speaker3 W and microSD; actual variants/condition/envelopes/controllers remain unverified.
- Preserve accepted ESP-IDF 6.1.x / Python 3.12 FastAPI role adapters, Bahasa Indonesia and approved prebuilt voice; patch/model/voice/controller/GPIO/endpoint pins remain prerequisite evidence, not facts inferred here. Proposed `/sijalak/v1` is a namespace contract, not a tested address.
- Input WAV PCM16 little-endian mono 16 kHz ≤8 s/256,044 canonical bytes; output normalized WAV PCM16 mono 24 kHz ≤15 s, download ≤768 KiB. Verify raw mic conversion and actual speaker audio; do not relabel incompatible codecs.
- PTT capture only while held; release/8 s ends capture. STOP/reset stops local capture/output, invalidates generation, requests server purge and discards stale/cross-session replies. Mic inactive at idle, playback, STOP/reset and child-local; local/server purge is not provider deletion.
- Session ≤4 turns/180 s; reply ≤35 words; poll 500 ms with original 30 s deadline; ≤5 new turns/min/device. Repeated demo sessions must retain these caps; retries/idempotency and budget admission remain bounded.
- Case: original Jalak Gembul, H140–160 mm/nominal150, four parts; actual simultaneous fit/service/OLED/audio access must be checked. Central70×55×80 and other reservations are not packing proof; invent no PCB mounting, coordinates or wire routing.
- Raw PLA target130–170 g/cap170 across all case parts, supports, brim, purge, coupons, reprints and other consumption; electronics excluded, assembled mass separate. Lower consumption accepted only with retained fit/quality. Future commissioned print/fit needs actual revision-linked evidence and approval, not an analytical estimate.
- Regulated external power first: candidate5 V/≥2 A is not a verified supply choice. Inspect actual modules, connections, rails/current/limits before energizing; never inject5 V into3.3 V logic or combine unverified supplies. Housing external≤40°C is a proposed conservative target, not a safety standard; stricter applicable component limits prevail.
- LiPo3.7 V/450 mAh≈1.665 Wh nominal is not a runtime guarantee. Keep unverified battery/charger/boost/load-sharing paths isolated; no charging or field-battery operation until power-path/protection/manufacturer limits are verified and separately approved.
- Rp500k additional prototype ceiling excludes owned electronics, printer purchase and VPS/API operations; selected250k case/100k controls-power-fasteners/150k reserve are allocations, not quotes. Quote actual print/access/supply costs before use; Rp100k/month operations is a placeholder requiring current role prices, FX, quotas and billing approval. Unknown/exhausted budget blocks calls.

## Proposed acceptance criteria — targets, not facts
- [ ] AC-01: fresh named `06-01` implementation approval and relevant separate permissions, accepted Phase 3/5 evidence and actual provider eligibility exist before their respective operations; otherwise BLOCKED.
- [ ] AC-02: a consenting adult's actual robot-mic utterance traverses all three real provider roles and is audible from the enclosed robot; profile, network, formats, stage timing and fallback labels are evidenced without a companion requirement.
- [ ] AC-03: actual closure/service/control paths, OLED states at0.5 m, unobstructed mic/speaker, retention/edges and proposed10° assembled tilt all directions pass; actual rails and approved physical limits pass before participant use.
- [ ] AC-04: STOP in capture/upload/poll/download/playback and reset suppress capture/output and late generations; accessible device/server audio/transcripts/context are purged, TTL10 min/result expiry60 s checked, provider retention separately disclosed.
- [ ] AC-05: ten consecutive smoke cycles READY→PTT→real reply→STOP/reset→fresh READY are recorded; a supervised60-minute external-regulated-power run includes Wi-Fi/OLED/audio workload and thermal samples at least every5 min, with no unsafe heat, instability or unexplained resets.
- [ ] AC-06: predeclared ≥30 eligible adult turn attempts on a stated network target≥90% intent fidelity for clear supported-language inputs, ≥95% complete real-AI technical success and p95 utterance-end→first robot-audio≤10 s. Report failures/censored no-audio attempts and denominators; fixed synthetic negatives are separate, not AI successes.
- [ ] AC-07: outage/TLS/auth/timeout/unsupported-model/malformed-codec/silence/oversize/duplicate/stale/reset/quota cases fail boundedly; controlled cases have0 unauthorized child/cloud calls,0 stale/cross-session playback and0 invalid controller actions. No quota-draining experiment or unlimited paid retry; zeros in fixtures are not universal guarantees.

## Edge coverage — future backstops
| Edge | Backstop / blocked outcome |
|---|---|
| Missing accepted source, actual profile/model, quote, permission or participant eligibility | Halt before affected work/call; never substitute assumptions or another plan's approval. |
| Closure pinches/blocks modules, unreadable OLED, echo/feedback, failed tilt or stale slice | De-energize/revise through owning plan; repeat fit/acoustic/material checks on actual revision. |
| Wrong rail, heating/smell/swelling/brownout or unknown battery path | Qualified operator stops immediately under approved isolation procedure; no firmware-only safety claim. |
| STOP/PTT/reset race, late download or disconnected network | On-device generation/state/purge evidence and labelled fault fixtures; no hidden capture or stale playback. |
| PC test/local recording looks convincing; selective metric exclusions or cost gaps | Explicit partial/mock/fallback classification; provenance-linked full-path evidence required, no readiness PASS from an evidence parser alone. |

## Prohibitions and assumptions
No questions, CLI/init/query/scripts/tests/build/flash/install, source/CAD/assets, printing, API/VPS/model/billing/field/Git or global changes now; only the three Markdown files may be created via `apply_patch`.
No fabricated measurements, source availability, approval, packing, battery runtime, provider retention, safety standard or literacy effect. Actual hardware/case/printing access, contracts/models/prices, network, codec, thermal/acoustic and participant evidence remain V; all root requirements/phases remain not executed.
