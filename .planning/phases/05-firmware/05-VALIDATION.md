---
phase: '05'
slug: firmware
status: draft
nyquist_compliant: false
wave_0_complete: false
created: '2026-10-09'
---

# Phase 05 — Future Validation Strategy

Planning supplement only. No test, firmware, binary, UI asset, content/SD fixture or hardware result produced; no commands or SDK/model calls run.
All six existing tasks, five firmware requirement IDs and phase threat references are mapped; planning coverage is not runtime/Nyquist compliance.
Sources: `.planning/phases/05-firmware/05-SPEC.md`, `.planning/phases/05-firmware/05-CONTEXT.md`, `.planning/phases/05-firmware/05-01-PLAN.md`, `.planning/phases/05-firmware/05-02-PLAN.md`, `.planning/phases/05-firmware/05-RESEARCH.md`, `.planning/research/AI-RESEARCH.md`, `.planning/VERIFICATION-STRATEGY.md`, `.opencode/gsd-core/templates/VALIDATION.md`.

## Test infrastructure — future only
| Property | Contract |
|----------|----------|
| Framework | Planned Python unittest approved bench harness and ESP-IDF6.1.x build; patch/toolchain/drivers/config unverified |
| Config / fixtures | Future `firmware/main/Kconfig.projbuild`; fixtures embedded in planned tests; human local manifest/PCM supplied only after approval |
| Quick command | `python3 -m unittest discover -s firmware/tests -p test_audio_contract.py -v` — actual conversion code/approved diagnostics required |
| Full suite | Applicable exact commands in the task map; no new runner or SDK-install command invented |
| File/runtime evidence | Every verification target below: No, future-only; runtime/feedback latency/actualFPS unknown/not measured |
| Setup authority | Named-plan G0 before setup; separate install/audit/pins, build/flash/hardware/signing/provisioning/server permissions; halt automatic downloads |

## Sampling rate — only after approval
- MAIN must obtain new named05-01/05-02 approvals outside execution before workflow/executor/Wave0 starts; YOLO planning or the other plan's approval cannot count.
- After each approved task run its mapped checks only within separately granted scope; approval receipts require human review, not just presence.
- After wave1 repeat applicable adult build/E2E/audio/fault checks; after wave2 repeat applicable local build/E2E/isolation/content checks and adult non-live regression checks where authorized.
- No Git/commit trigger; adult provider-dependent reruns are not automatic and need actual eligible consenting-adult/provider/data/billing allowance.
- Before future runtime sign-off review actual image/profile/wipe/mic/egress/audio evidence, human observations and unresolved gates; no field use authorized by sampling.
- Feedback latency unknown.30s deadline/10–15FPS/≤90s handoff are selected runtime targets, not measured harness duration or passing results.

## Per-task verification map — exact existing-plan commands, future only
| Task ID | Plan | Wave | Requirement IDs | Threat Ref | Secure behavior / failure meaning | Test type | Exact future automated command | File Exists | Status |
|---------|------|------|-----------------|------------|-----------------------------------|-----------|--------------------------------|-------------|--------|
| 05-01-01 | 05-01 | 1 | FW-01, FW-02, FW-03, FW-04 | T-05-01, T-05-SC | New named approval before workflow and separate hardware/install/provider scopes; fail inherited/missing approval; existence is not authority | Receipt existence + manual gate | `test -s .planning/phases/05-firmware/05-01-AUTHORIZATION.md` | No — future receipt | pending |
| 05-01-02 | 05-01 | 1 | FW-01, FW-02, FW-03, FW-04 | T-05-01, T-05-02, T-05-03, T-05-04 | Actual mic→VPS→speaker/state tracer; fail absent board/supply/toolchain/permission/real device, mock-as-real or stop/TLS/codec/heap violation | Approved build + real device E2E | `idf.py -C firmware -B firmware/build/adult-lab -D SIJALAK_PROFILE=adult-lab build`<br>`python3 -m unittest discover -s firmware/tests -p test_adult_e2e.py -v` | No — future project/test | pending |
| 05-01-03 | 05-01 | 1 | FW-01, FW-02, FW-03, FW-04 | T-05-01, T-05-02, T-05-03, T-05-04 | Actual conversion/state/fault code; fail unrelated test reimplementation, idle/playback/stopped capture, stale output, bound/TLS/heap faults or invented timing | PCM/state/network bench negatives | `python3 -m unittest discover -s firmware/tests -p test_audio_contract.py -v`<br>`python3 -m unittest discover -s firmware/tests -p test_adult_faults.py -v` | No — future tests | pending |
| 05-02-01 | 05-02 | 2 | FW-05, FW-01, FW-03, FW-04 | T-05-05, T-05-06, T-05-SC | New05-02 approval with separate signing/content/provisioning/revocation scope; fail inherited05-01 consent or missing external authority | Receipt existence + manual gate | `test -s .planning/phases/05-firmware/05-02-AUTHORIZATION.md` | No — future receipt | pending |
| 05-02-02 | 05-02 | 2 | FW-05, FW-01, FW-03, FW-04 | T-05-05, T-05-06, T-05-07, T-05-08, T-05-09 | Distinct human-content local path; fail absent license/capacity/bench, adult credential not removed/revoked before deployment, capture/cloud path or false live/holding claim | Approved local build + device E2E | `idf.py -C firmware -B firmware/build/child-local -D SIJALAK_PROFILE=child-local build`<br>`python3 -m unittest discover -s firmware/tests -p test_child_local_e2e.py -v` | No — future project/test/content | pending |
| 05-02-03 | 05-02 | 2 | FW-05, FW-01, FW-03, FW-04 | T-05-05, T-05-06, T-05-07, T-05-08, T-05-09 | Inspected image/wipe/spoof/SD/reset negatives; fail any mic/cloud/child-data egress, upgrade path, stale credential, unsafe media read/update or invented provenance/safety | Binary/credential/egress/content negatives | `python3 -m unittest discover -s firmware/tests -p test_child_isolation.py -v`<br>`python3 -m unittest discover -s firmware/tests -p test_child_content.py -v` | No — future tests | pending |

## Wave 0 requirements — future preparation within approved task order
- [ ] 05-01 new G0 and actual Phase01/04 readiness first;05-02 needs its own new G0 after05-01. No preapproval setup or implicit install.
- [ ] `firmware/tests/test_adult_e2e.py` — approved bench identity/control/state fixtures, accepted protocol and separately authorized actual mic/provider/speaker case; mocks cannot pass real E2E.
- [ ] `firmware/tests/test_audio_contract.py` — synthetic silence/ramp/clipping/endian/max/malformed WAV and raw-slot alignment cases conditional on actual Phase01 codec/clock facts; exercise actual conversion code.
- [ ] `firmware/tests/test_adult_faults.py` — approved protocol fault mocks for TLS/auth/ownership/idempotency, STOP/release/reset/stale generation, rate/time/format/download/heap bounds and microphone inactivity.
- [ ] `firmware/tests/test_child_local_e2e.py` — human-local START/STOP/choice/handoff fixture, no capture/network path, actual signed profile and predeployment credential clearance.
- [ ] `firmware/tests/test_child_isolation.py` — actual binary/boot/control/token/mode spoof, separately authorized inventoried NVS/flash/SD removal plus server revocation, microphone/cloud/child-data egress negatives.
- [ ] `firmware/tests/test_child_content.py` — bounded hash/offset/format/path/capacity/media/interrupted-update/unavailable-book/reset cases against actual loader.
- [ ] Future `content/child-local/manifest.json` and `content/child-local/hooks.pcm` must be owner-supplied human-authored/recorded/licensed, capacity-checked and DEMO or verified holdings; no generated child data/script/audio now.
- [ ] Embed conditional protocol/PCM fixtures in already-planned files only; unknown pins/controller/power/SD/free-memory halt affected cases, never silently skip-as-green.
- [ ] Dependencies/toolchain/drivers require separate install approval and actual audit/pins before setup; no SDK download, global credential borrowing or destructive NVS/fuse action inferred.
- [ ] Wave0 follows each approval and existing tracer-first order; preparation is not evidence that any test exists, ran or passed.

## Manual-only verifications — cannot be replaced by grep
| Behavior | Requirement / threat | Why manual | Future evidence required |
|----------|----------------------|------------|--------------------------|
| New named-plan and external scopes | All mapped IDs; T-05-SC | Only owner grants implementation/install/hardware/signing/server authority | Reviewed05-01/05-02 receipts before workflow; no inherited/automatic approval |
| Actual mic/OLED/GPIO/clocks/DMA/PSRAM and regulated supply | FW-02, FW-03; T-05-02, T-05-04 | Public two-I2S docs/owned labels cannot inspect hardware or safety | Phase01 measured board/driver/slot/controller/power evidence, safe approved bench and STOP checks; no invented pins/8MB-free assumption |
| Actual adult provider/account/schema/model/locale and consent | FW-01, FW-04; T-05-01 | Indexed candidates and direct VPS do not establish eligibility | Accepted Phase04 actual contract and eligible restricted18+ R&D approval; no child/proxy/guardian/paid bypass |
| Human content/license, SD capacity and actual books | FW-05; T-05-07, T-05-08 | Hashes cannot prove author rights or shelf availability | Owner-reviewed human sources/media/capacity; actual Phase02 catalog for field labels, DEMO bench distinctly marked |
| Child image authority, adult credential removal/revocation | FW-01, FW-04; T-05-05, T-05-06 | File grep cannot revoke a credential or sanitize actual storage | Separate server/NVS/flash/SD/provisioning authority and verified removal/revocation before child flash/start; preserve unrelated/calibration data, document sanitization limits |
| Actual mic/no-egress, audible states and quiet handoff | FW-01, FW-03, FW-05; T-05-02, T-05-08, T-05-09 | Build success/constants alone cannot prove running hardware behavior | Approved adult-operated bench observation/instrumentation plus mapped tests; no child participant/data or field permission here |

## Failure and evidence discipline
- Zero stale playback/capture/network-AI/child-data egress required in chosen negatives; token/mode flips cannot enable child cloud. Signed deployment is not unverified hardware secure-boot proof.
- Mock/recorded/local fallback never counts as actual adult AI success; local content is disclosed, not AI. Unavailable book/media errors cannot invent holdings or invoke cloud.
- No private samples/child data recorded now; later evidence uses approved case IDs/metadata and honest observed limitations. Runtime/P95/safety/reading benefit not inferred from planning.

## Validation sign-off — runtime pending
- [ ] All mapped artifacts exist and authorized checks ran; no missing/empty/skipped fixture set counted green.
- [ ] Wave0, after-task/wave sampling, manual permissions/hardware/credential gates and actual feedback latency evidenced without Git triggers.
- [ ] Distinct profiles, real adult path and child-local wipe/mic/egress/content evidence reviewed; field/assembled safety/consent remains separately gated.
- [ ] Only separately authorized later runtime validation may change status/flags; this supplement leaves draft/false/false unchanged.
Approval: pending. Planning task/REQ/threat/command coverage can be checked separately; no execution authority, Nyquist compliance or measured runtime established.
