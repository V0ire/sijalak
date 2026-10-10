# Phase 05 — Recorded Research Supplement

Created: 2026-10-09. Planning only; uses existing local evidence, not new browsing, hardware inspection or implementation.
No SDK/model call, command, test, build/flash, install, Git, hook, global change, private data or SD content generated/accessed.

## Evidence levels
U = owner report; D-public = previously recorded public documentation; D-indexed = recorded Context7 official excerpts; A = selected contract; V = actual verification pending.
An SDK reference or owned-part label does not prove driver compatibility, safe power, actual capacity, latency or a working robot.
| Topic | Evidence/source | What remains unverified |
|-------|-----------------|-------------------------|
| ESP32-S3 N16R8, INMP441, MAX98357A, OLED and microSD owned | U; canonical owner reports carried into05 SPEC/CONTEXT | Actual variants, GPIO, clocks/slots, display controller, free heap/PSRAM, SD capacity and power |
| Stable S3 I2S documentation labelled6.1, two controllers, channels/clocks/DMA | D-public; root research records https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/i2s.html | Exact approved6.1.x patch/toolchain and real mic24/32-bit alignment/conversion/driver configuration |
| OLED128×64/1.3-inch report | U/A; existing05 SPEC/CONTEXT | SH1106 versus SSD1306, address/envelope/readability; no GPIO/wiring layout may be invented |
| VPS/API and speech stages; “Gemini, presumably” | U; `.planning/research/AI-RESEARCH.md` | Actual namespace binding/schema/auth/model/account/locale/prices and accepted Phase04 endpoint |
| Candidates `gemini-3.5-transcribe`, `gemini-3.1-flash-lite`, `gemini-3.8-flash-lite-tts`, `Kore` | D-indexed; recorded official Context7 excerpts, not account/model tests | Supported actual IDs, price, locale and output codec; firmware never carries provider secret |
| Developer API18+ and no child-directed/likely-under18 client | D-public; recorded Google terms dated2026-04-28, read2026-10-09; https://ai.google.dev/gemini-api/terms | Adult internal R&D client eligibility before calls; no child exception presumed |
| Two signed deployment profiles, half-duplex/state/buffer targets | A; existing05 plans/spec/context | No binary, signing/secure-storage capability, capture/egress test, safe powered bench or performance evidence |

## Preserved device and local-content boundaries
- Adult path: real robot PTT/mic→approved own VPS/STT/chat/TTS→actual speaker and OLED; an operational laptop/phone is not required.
- Direct VPS/proxy/guardian/paid tier/adult relay cannot bypass the recorded Gemini child restriction; no child script generation through Gemini.
- Child-local is a distinct binary/permission/deployment path, not an adult image with a mode flag; no microphone, cloud AI, visitor history or child-data egress.
- Before child deployment, separately authorize and verify adult credential removal from inventoried NVS/flash/SD/provisioning and backend revocation; no unapproved erase/fuse operation.
- Human-authored/recorded or appropriately licensed local hooks/choices only; DEMO bench content cannot claim actual TBM holdings. Actual catalog depends on Phase02.
- SD capacity/free space/media/format/provenance remain V; validate bounded manifest/hash/offsets/clip sizes and safe activation rather than assume storage is available.
- WAV input PCM16/16k mono ≤8s/256,044 bytes; output PCM16/24k mono ≤15s/720,044 bytes, hard768KiB. Raw mic slots require actual conversion.
- Hold/release PTT plus separate STOP; no mic on idle/playback/stop/child. TLS/ownership/generation,500ms polling/30s deadline,4 turns/180s and35-word replies remain enforced.
- Appropriate DMA RAM and measured PSRAM reserve required; nominal8MB is not all free. OLED10–15FPS and quiet local handoff≤90s are targets, not measured results.

## Future evidence and permission gates
- Phase01 supplies actual inventory/contracts/safe-bench evidence; Phase04 must be accepted before05-01 real adult E2E;05-02 follows05-01.
- New named-plan approval precedes workflow/executor launch; planning YOLO or05-01 approval cannot authorize05-02.
- Install/dependency pin/legitimacy, hardware/build/flash, signing, server revocation, provider/data and billing scopes stay separate; no automatic tool/driver downloads.
- Regulated supply, physical STOP, clocks/display/codec and actual buffers need approved hardware verification; owned LiPo capacity proves neither safety nor runtime.
- Rp500k additional hardware and unpriced Rp100k/month operation ceiling are distinct and not spend permission. Child-local bench success is not field/pilot consent.
- Synthetic/approved adult bench diagnostics only; no private child data. Actual capture/egress/credential-wipe evidence and feedback latency remain pending, not green.

## Source handoff
Sources: `.planning/research/AI-RESEARCH.md`, `.planning/AI-SPEC.md`, `.planning/VERIFICATION-STRATEGY.md`, `.planning/phases/05-firmware/05-SPEC.md`, `.planning/phases/05-firmware/05-CONTEXT.md`, `.planning/phases/05-firmware/05-01-PLAN.md`, `.planning/phases/05-firmware/05-02-PLAN.md`.
FW-01…FW-05 scope is unchanged. `.planning/phases/05-firmware/05-VALIDATION.md` maps prospective checks; no existing plan/root document/code/asset is changed by this supplement.
