---
phase: exploration
plan: 'sw'
type: execute
wave: 1
depends_on: []
files_modified:
  - firmware/
  - backend/
  - tools/
autonomous: false
requirements: [FW-03, FW-04, API-02, AI-03]
must_haves:
  truths:
    - "Offline logic can be built and tested on the host without ESP32 hardware, credentials, or network."
    - "Hardware/API/field gates remain BLOCKED and are never reported as verified."
  artifacts:
    - {path: firmware/logic, provides: "Portable eye/state/audio/gesture logic with host tests"}
    - {path: backend, provides: "Stub-provider gateway logic with host tests"}
  key_links:
    - {from: firmware, to: backend, via: "Shared JSON turn contract exercised by mock server"}
---

<objective>
Execute the owner-authorized OFFLINE software workstream (SW-FW-01..05, SW-BE-01..02) that is independent of physical hardware and external providers. Real code, real host builds, real automated tests. Do not claim hardware/API/field verification.
</objective>

<results status="partial">
Executed 2026-10-09 under AUTH-U-03. Evidence: `.planning/EVIDENCE/OFFLINE-EVIDENCE.md`.

- SW-FW-01..05 — **implemented + compiled + simulated**: `firmware/components/jalak_logic`
  (expression/framebuffer/state_machine/audio/api_client) + `firmware/components/jalak_hal`
  mocks (incl. `MockTransport`). Host build via cmake+ninja, `TMPDIR=<repo>/.buildtmp`.
  `ctest`: **3/3 passed** (jalak_logic, jalak_hal, jalak_api).
- SW-BE-01..02 — **implemented + compiled + simulated**: `backend/jalak_backend`.
  `rtk pytest -q`: **17 passed** (incl. firmware↔backend contract sync).
- Defect fixed: `draw_face()` double-counted `painted` on wink eyes; corrected and
  verified across all 11 moods.

Gate impact: **none**. G1–G5 remain BLOCKED; phase goal NOT_VERIFIED; requirements 0/37.
Remaining unverified (out of offline scope): ESP32-S3 target image, real OLED/mic/amp,
real HTTP/TLS transport, live provider.
</results>
