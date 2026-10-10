# Requirements: Si Jalak Bedas v0.1

Defined2026-10-09. Auto-selected planning baseline, **not implemented or field-validated**. Core value: interaction that hands attention to reading rather than keeping it on the robot. All boxes intentionally unchecked. See `DECISIONS.md` for U/D/A/V evidence and `VERIFICATION-STRATEGY.md` for checks.

## v1 Requirements

### Baseline and contracts
- [ ] **BAS-01**: Owner/operator can inspect a provenance-labelled inventory of actual component variants/dimensions and service contracts; unknowns cannot masquerade as tested facts.
- [ ] **BAS-02**: Prototype work respects Rp500k additional-material cap and separately authorized operating costs, safe power assumptions, and planning-only/implementation permissions.
- [ ] **BAS-03**: STT/chat/TTS contracts, normalized audio formats, rate/size limits and model-version gates are documented before device integration.

### Field research
- [ ] **RSR-01**: TBM partnership and voluntary/guardian/participant procedures are confirmed before any contact/observation involving participants is executed.
- [ ] **RSR-02**: Baseline observations and short nonleading interviews distinguish observations, participant accounts and interpretations without personal identifiers/recordings.
- [ ] **RSR-03**: Pilot cohort, available-book catalog and reading-start proxy are chosen from actual site evidence; provisional10–14 cohort may change.

### Industrial Design, 3D Printable Enclosure & Character Design
- [ ] **IDN-01**: At least three distinct forms are compared; an original hybrid has documented silhouette/proportion rationale, not copied product geometry.
- [ ] **IDN-02**: Nominal150 mm envelope and usable-volume reservations are checked against measured parts and service access before CAD freeze; no volume-only fit claim.
- [ ] **IDN-03**: OLED active window, face zone, speaker opening and microphone access remain distinguishable and adaptable to actual modules; mask is not claimed to enlarge the display.
- [ ] **IDN-04**: Main body, crown, bottom service closure and visor can be assembled/reopened without fragile exposed clips or loose small decorative parts; PCB mounting/wiring layout deferred.
- [ ] **IDN-05**: FDM profile/material/orientations are documented; one complete case print-set including supports/brim/purge targets130–170g, cap170g; electronics excluded. Development coupons/reprints tracked separately and included in cumulative prototype cash costs.
- [ ] **IDN-06**: Overhang/bridge, heat, limb-root, tip-over, sharp-edge and support-removal risks have explicit mitigation and later checks; no support-free or toy-certification claim.
- [ ] **IDN-07**: Parametric CAD → prototype → slicing → fit-check → revision gates and dimensions to verify are recorded before any model/print execution.
- [ ] **IDN-08**: Character identity has one OLED focal region, blunt horn/crest and stub-wing features, readable states and low-finishing appearance criteria, tested after prototyping.

### VPS API
- [ ] **API-01**: An isolated backend adapter provides a real bounded STT→chat→TTS adult-demo turn without modifying unrelated VPS/OpenCode/9Router services.
- [ ] **API-02**: Device authentication, TLS server verification, per-device session ownership, idempotency and payload/rate limits prevent unauthorized/cross-device requests.
- [ ] **API-03**: Stop/reset invalidates stale replies and isolates sessions; transient audio/transcripts/dialog state are deleted locally/backend according to policy; provider retention is separately verified.
- [ ] **API-04**: Metadata-only latency/error/usage reporting and spending/time limits are observable; upstream unavailable/timeout/quota errors fail boundedly and never fake AI success.

### AI and eligibility
- [ ] **AI-01**: Live Gemini Developer API use is gated to an eligible, restricted adult internal R&D client; no under-18/child-directed pilot calls, even through a proxy or with guardian consent.
- [ ] **AI-02**: Bahasa Indonesia STT/dialog/TTS adapters use verified pinned roles/models/voice/formats before live requests; owner's “Gemini presumably” is not treated as tested provisioning.
- [ ] **AI-03**: Replies are short, contextual and persona-bounded; book IDs come only from a curated allowlist; unsafe/injected/unsupported requests cannot produce unvalidated actions or invented TBM holdings.
- [ ] **AI-04**: Audio capture is explicit and privacy-visible; no profiles, voice cloning, speaker/emotion identification, raw-content logs or persistent cross-visitor memory by default.
- [ ] **AI-05**: Any local fallback is disclosed and excluded from AI-success counts; session/word/audio/cost caps stop unbounded retries/conversation.
- [ ] **AI-06**: Synthetic/consenting-adult eval sets, rubrics, latency/error/cost and negative-case evidence are defined; no dataset or results are fabricated.

### Firmware and interaction
- [ ] **FW-01**: ESP-IDF build/deployment profiles separate adult-lab networked AI and child-local mode; child mode cannot unlock live Gemini through controls or profile spoofing.
- [ ] **FW-02**: INMP441 input and MAX98357A output follow verified standard-I2S/sample conversion and bounded buffers; no recording while speaker plays in half-duplex mode.
- [ ] **FW-03**: Explicit start/PTT and stop controls plus OLED ready/listening/processing/speaking/stopped/error/book-handoff states make activity understandable without color alone.
- [ ] **FW-04**: Network client uses session/turn IDs, bounded upload/download/polling, cancellation and reset; wrong/stale session audio is never played.
- [ ] **FW-05**: Child-local/offline curated reading invitations run without voice capture/cloud AI, never pretend to be live AI, and can hand attention away to a real book.

### Integrated demo
- [ ] **INT-01**: A consenting adult's voice captured by robot reaches real AI through VPS and returns through robot speaker; computer-only tests/recordings are labelled partial/fallback.
- [ ] **INT-02**: Power, thermal, casing access, acoustic feedback and operator stop checks pass before demonstration; owned components alone do not establish safety.
- [ ] **INT-03**: Repeatability, latency, reset/fault/quota behavior and safe60-minute powered demo are measured against proposed acceptance, not claimed from planning.

### Literacy pilot
- [ ] **PIL-01**: Child pilot uses a technically isolated local profile, verified provider/data eligibility, actual partner permission, guardian consent and child willingness; no Gemini bypass.
- [ ] **PIL-02**: Human-authored/recorded or licensed short hooks and bounded choices point to available curated books; operator can update/disable content without identity profiles.
- [ ] **PIL-03**: Matched no-robot comparison, reading-start proxy, assistance, distractions and operator burden are reported with denominators; pilot is not claimed to prove comprehension or long-term habits.

### Handover and iteration
- [ ] **OPS-01**: Operator receives startup/stop/content/charging-cleaning/safe-storage instructions, profiles, rollback/incident procedure and accountable maintainer without exposing secrets.
- [ ] **OPS-02**: Evidence supports revise/continue/stop decisions and pending provider/field/design risks; unproven claims cannot enter rollout documentation.

## Deferred v2 / exclusions
Locomotion/servos, presence camera/always-listening wakeword, flexible live child Gemini conversation, points/leaderboards, persistent user accounts, broad book RAG/web browsing, cloned voices, unrestricted Sundanese/code-switching, mass-manufacturing mounts/wiring and multi-unit dashboard are deferred. Timer is optional later if field need warrants it, not a proxy for reading.

## Traceability
| Requirement | Phase | Status |
|-------------|-------|--------|
| BAS-01 | 1 | Pending |
| BAS-02 | 1 | Pending |
| BAS-03 | 1 | Pending |
| RSR-01 | 2 | Pending validation |
| RSR-02 | 2 | Pending validation |
| RSR-03 | 2 | Pending validation |
| IDN-01 | 3 | Pending |
| IDN-02 | 3 | Pending validation |
| IDN-03 | 3 | Pending validation |
| IDN-04 | 3 | Pending validation |
| IDN-05 | 3 | Pending validation |
| IDN-06 | 3 | Pending validation |
| IDN-07 | 3 | Pending |
| IDN-08 | 3 | Pending validation |
| API-01 | 4 | Pending |
| API-02 | 4 | Pending |
| API-03 | 4 | Pending |
| API-04 | 4 | Pending |
| AI-01 | 4 | Pending eligibility |
| AI-02 | 4 | Pending validation |
| AI-03 | 4 | Pending |
| AI-04 | 4 | Pending |
| AI-05 | 4 | Pending |
| AI-06 | 4 | Pending |
| FW-01 | 5 | Pending |
| FW-02 | 5 | Pending |
| FW-03 | 5 | Pending |
| FW-04 | 5 | Pending |
| FW-05 | 5 | Pending |
| INT-01 | 6 | Pending validation |
| INT-02 | 6 | Pending validation |
| INT-03 | 6 | Pending validation |
| PIL-01 | 7 | Pending validation |
| PIL-02 | 7 | Pending validation |
| PIL-03 | 7 | Pending validation |
| OPS-01 | 8 | Pending |
| OPS-02 | 8 | Pending validation |

Coverage: **37 requirements, 37 mapped once, 0 unmapped, 0 implemented/validated**. Native plan checks assess planning quality only.
