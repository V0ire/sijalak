# Phase 05: ESP32 Voice and Expression — Context

Gathered: 2026-10-09. YOLO planning only; implementation and device access remain blocked.
Source precedence: current instruction → canonical decisions/contracts → recorded local research → internal task choices.

<domain>
## Phase boundary
After Phases 1 and 4, actual adult robot audio/state E2E precedes expansion; separate child-local binary follows. No operational laptop/phone during a turn.
</domain>

<decisions>
## Decisions — numbered, with rationale
Global D IDs retain canonical meanings; D-19 is the shared authorization lock, D-23…D-26 record firmware-specific locks.
| ID | Decision | Rationale / evidence boundary |
|----|----------|-------------------------------|
| D-01 | No child Gemini or relayed child speech; adult demo restricted internal 18+ R&D with eligibility gate | Developer API users/audience restriction; proxy, consent and paid tier are not exceptions |
| D-03 | PTT hold/release plus separate STOP, half-duplex, no mic on playback/idle/stop | Explicit control and bounded acoustic/privacy risk; exact buttons unknown |
| D-04 | WAV16k PCM16 mono input ≤8 s/256,044 bytes; WAV24k output ≤15 s/720,044 bytes, hard768 KiB | Device normalization and validation are required; provider/raw mic formats differ |
| D-07 | ESP-IDF6.1.x; approved exact patch/toolchain before build | Documented two S3 I2S controllers do not verify owned module/pin/slot behavior |
| D-09 | Bahasa Indonesia; adult prebuilt voice only after approval | No cloning; human/appropriately licensed child audio only |
| D-10 | Adult ≤4 turns/180 s, ≤35-word reply, bounded handoff | Attention/cost/audio caps are controller invariants, not prompt suggestions |
| D-11 | Child-local human hook + limited choices → actual book → quiet | Tests literacy hypothesis without Gemini; DEMO catalog remains explicitly non-field |
| D-15 | Verified regulated external supply for bench; battery operation/charging gated | Owned450 mAh LiPo gives no safety/runtime proof |
| D-18 | Distinct signed/deployment adult/child binaries and backend permission; remove/revoke adult credentials before child deployment | Mode flags and visitor controls cannot upgrade capability; no capture or child-data egress |
| D-19 | New explicit named-plan implementation approval before workflow; all other permissions separate | Planning YOLO never authorizes execution; 05-01 approval cannot authorize05-02 |
| D-23 | Measure mic alignment/clocks, OLED SH1106 vs SSD1306, GPIO, safe power and DMA/RAM/PSRAM availability | No invented wiring or assumption all8 MBPSRAM is free; reject unresolved configuration |
| D-24 | Actual monochrome state cues, BOOT through handoff/error, target10–15 FPS | Distinguish real microphone activity; UI must not starve audio/network; no assets now |
| D-25 | Child image links no capture/cloud path; start/cycle/stop operates local provenance-checked hooks | No Gemini generation of child scripts, hidden AI fallback, profiles, guilt or invented holdings |
| D-26 | Verify SD capacity/free space/media before provisioning; bounded hashed package and atomic activation | Unknown capacity/corruption must fail visibly rather than truncate, execute paths or invoke cloud |
### Agent discretion
Internal state/test structure and bounded local menu timing within the locked flows; no discretion over pin guesses, security, eligibility or permission gates.
</decisions>

<canonical_refs>
## Canonical references
- `.planning/PROJECT.md`, `.planning/DECISIONS.md`, `.planning/REQUIREMENTS.md`, `.planning/ROADMAP.md` — dependencies, IDs and provenance.
- `.planning/ARCHITECTURE.md` §§3–8; `.planning/AI-SPEC.md` §§2,4–9; `.planning/research/AI-RESEARCH.md` — hardware/protocol/provider contracts.
- `.planning/USER-FLOWS.md` A/B/C; `.planning/MVP.md`; `.planning/VERIFICATION-STRATEGY.md` — user paths, real E2E and evidence boundaries.
- `.planning/phases/05-firmware/05-SPEC.md`; `.planning/phases/04-vps-ai/04-SPEC.md` — device targets and prerequisite gateway contract.
</canonical_refs>

## Existing inputs / future facts
No firmware/tests/UI/assets/SD content created or device access performed. Actual pins, controller, slots, free heap, SD capacity and safe power remain V.
Phase 1 provides verified board/contract evidence; Phase 4 provides accepted owned-session gateway. Actual host/models/rates/locale not guessed.
Phase 2 catalog is required only for actual field labels/deployment; human-authored DEMO bench content may precede it but cannot claim TBM availability.
Future install, build/flash/hardware, server/API/provider/billing, Git, printing and field permissions are separate; no workflow may launch merely to obtain G0 approval.

<deferred>
## Deferred ideas
Enclosure/assembled safety/60-minute demo (03/06), actual field participation (02/07); no detailed wiring/mounting layout, always-listening, motors, profiles, RAG/tools or cloning.
Live child AI needs a separately eligible offering/contract and new review. Nothing here changes root roadmap/config or approves that future work.
</deferred>
