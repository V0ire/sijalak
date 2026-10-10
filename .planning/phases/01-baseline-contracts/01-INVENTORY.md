# 01 Inventory — revision 2 DOC-FIRST packet

**Scope:** Tasks 2–3 documentary inventory. **G1: BLOCKED; readiness: BLOCKED; phase_goal: NOT_VERIFIED; integration_admission: false.** No physical inspection or owner-specific measurement occurred. Ownership is reported, not independently confirmed.

U = owner report; D = recorded public documentation; A = proposal; V = pending proof. These classes do not certify an actual part. Source IDs resolve in [01-VERIFICATION.md](01-VERIFICATION.md#source-register). Authorization is MAIN's [AUTH-U-02](01-AUTHORIZATION.md), not a new receipt.

## OLED source separation

| item_id | reported_part | public_reference_value | proposed_value | actual_product_label | actual_variant/controller | measured_W_D_H_mm | method | uncertainty | condition | source_id | evidence_class | unresolved_gate |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| INV-OLED-01 | Owned OLED 1.3-inch 128×64 | Representative Waveshare OLED (A), SH1106, 128×64; board 40.5×37.5 mm; active area 29.42×14.70 mm; owner match NOT established | Use actual visible area/envelope to constrain later bezel/service access; nominal case height 150 mm, torso 112×102×122 mm and clear central reservation 70×55×80 mm are A design inputs | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN — no owner measurement | UNKNOWN — no measurement uncertainty supplied | UNKNOWN | SRC-U04; SRC-OLED-C2; SRC-PACK-A; AUTH-U-02 | U report / D representative / A proposal / V actual | G1: actual label/controller, envelope, active-area offset, thickness, connector protrusions and service clearance need separately authorized owner-specific evidence; fit freeze BLOCKED |

The D board outline has only two published dimensions in the local record. It is not a measured W×D×H tuple, does not establish thickness and does not identify the owned display. SH1106 is the representative product's controller, not the owner's controller. Approximate 30×15 mm active area is a rounded reference, not a larger face window. The suffix “(A)” in “Waveshare OLED (A)” is a product designation; its published facts have evidence class D, not assumption class A.

## Reference and proposal revisions

- SRC-OLED-C2 is the prior 2026-10-09 public-document record at `../../research/INDUSTRIAL-RESEARCH.md` §5/C2; URL https://www.waveshare.com/1.3inch-OLED-A.htm. This run read the local record; no fresh retrieval.
- SRC-U04 is `../../DECISIONS.md` U-04 plus `../../PROJECT.md` §§Arahan pengguna/Unknowns, dated 2026-10-09. Ownership and part names are reports, not inspected variants.
- SRC-PACK-A is `../../DECISIONS.md` D-13 and `../../PROJECT.md` industrial parameters. Four proposed case pieces: body/torso, crown, base/service cap, visor/bezel. None exists as verified geometry here.
- The research §6 central 70×45×75 mm reservation is a superseded A proposal; the current 70×55×80 mm reservation is also A. Neither is actual fit evidence. No detailed mounts, pins or wiring are frozen.

## Downstream disposition

INV-OLED-01 → [CON-OLED-01](01-CONTRACTS.md#oled-device-contract-dependency) → [SAFE-OLED-01](01-SAFETY-BUDGET.md#oled-power-cost-and-fit-disposition) → [tracer BAS rows](01-VERIFICATION.md#source-to-readiness-tracer). Public dimensions cannot authorize CAD, power or electrical compatibility. Required actual fields remain V/UNKNOWN; G1 BLOCKED.

## Reported electronics — actual evidence absent

The actual columns below are all V/UNKNOWN: no labels, variants, measurement method/uncertainty, condition or electrical compatibility were inspected. SRC-U04 consolidates DECISIONS U-04 with PROJECT owner-part list and INDUSTRIAL-RESEARCH §1. Charging and conversion are distinct functional reports; their physical count or combination on one board is UNKNOWN. Reported names/ratings cannot establish size, working condition or pins.

| item_id | reported_part | public_reference_value | proposed_value | actual_product_label | actual_variant/controller | measured_W_D_H_mm | method | uncertainty | condition | source_id | evidence_class | unresolved_gate |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| INV-ESP-01 | Owned ESP32-S3 N16R8 | C1 representative DevKitC-1 v1.0: length 62.74 mm, width 25.40 mm; height not supplied | ESP-IDF 6.1.x candidate; later verified memory/board constraints | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-U04; SRC-ESP-C1; SRC-ARCH-A | U/D/A/V | G1: actual board/module label, memory configuration, footprint including headers/USB, usable memory, revision/secure storage and supported pins |
| INV-MIC-01 | Owned INMP441 microphone | No owner-matched datasheet supplied | Bounded half-duplex standard-I2S input with verified normalization | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-U04; SRC-ARCH-A | U/A/V | G1: actual breakout/voltage, acoustic opening, pin/clock/slot alignment and conversion evidence |
| INV-AMP-01 | Owned MAX98357A amplifier | No owner-matched module evidence supplied | Verified audio output and gain/rail/load compatibility later | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-U04; SRC-ARCH-A | U/A/V | G1/G3: actual module/rails, gain, I2S compatibility and speaker-load/protection evidence |
| INV-SPK-01 | Owned 3 W speaker | Reported rating only; no representative dimensions used | Distinct outward grille and safe later volume limits | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-U04; SRC-ARCH-A | U/A/V | G1/G3: label, impedance, diameter/depth, mounting/protrusions, acoustics and amplifier compatibility |
| INV-SD-MOD-01 | microSD reported; module/adapter form unspecified | No actual adapter/module evidence | Local-content storage candidate; service access required | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-U04; SRC-ARCH-A | U/A/V | G1: identify whether reader/breakout is owned, bus/voltage/connector footprint, insertion/removal clearance |
| INV-SD-CARD-01 | microSD reported; actual card existence/capacity not separately established | No capacity/speed/filesystem evidence supplied | Local licensed/human-authored assets if storage is verified | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-U04; SRC-ARCH-A | U/A/V | G1: distinguish physical card from reader, source capacity/speed/format/condition; no capacity guessed |
| INV-CELL-01 | Owned LiPo about 450 mAh / 3.7 V | Owner-reported nominal ratings only | External power first; cell use closed | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-U04; SRC-POLICY-A | U/A/V | G1/G3: actual cell label/envelope, chemistry/protection, connector/polarity, condition and manufacturer safety evidence |
| INV-CHG-01 | Charging module reported owned | No actual module datasheet supplied | No charge or load-sharing use before safety evidence | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-U04; SRC-ARCH-A | U/A/V | G1/G3: actual module/controller, limits/protection, charge profile/power-path and thermal evidence |
| INV-CONV-01 | Conversion module reported owned | No actual regulator/boost module evidence supplied | Verified rails only; no combining unknown supplies | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-U04; SRC-ARCH-A | U/A/V | G1/G3: actual topology/rails/current/protection/efficiency/connector and compatibility evidence |

N16R8 is a reported memory configuration, not a development-board identity or footprint. Neither the example C1 outline nor nominal memory establishes installed/usable application memory or pin layout. Speaker rating and cell capacity likewise do not establish envelope or safe operation. An SD reader is not a storage-capacity claim.

## Unconfirmed controls, supply and fabrication inputs

These are proposals or uncertain planning reports, **not reported owned electronics**. Their actual fields remain UNKNOWN independently of the proposal.

| item_id | reported_part | public_reference_value | proposed_value | actual_product_label | actual_variant/controller | measured_W_D_H_mm | method | uncertainty | condition | source_id | evidence_class | unresolved_gate |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| INV-CTRL-01 | Ownership/actual buttons UNKNOWN | None sourced | PTT/start plus separate STOP, accessible and explicit | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-POLICY-A D-03; SRC-ARCH-A | A/V | G1: actual controls/envelopes/actuation/ratings; later reach/usability evidence |
| INV-SUPPLY-01 | Ownership/actual supply UNKNOWN | No actual supply evidence | Regulated external 5 V/≥2 A candidate, subject to verified rails | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-ARCH-A §7; SRC-POLICY-A D-15 | A/V | G1/G3: rated source/cable/rails/protection/thermal evidence and separate power permission |
| INV-PRINT-01 | Adventurer 5-series, probably 5M planned; ownership/access UNKNOWN | Representative 5M build volume 220×220×220 mm and installed 0.4 mm nozzle; not owner's configuration | Later validated print profile/access/ventilation | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-U11; SRC-PRINT-D | U/D/A/V | G2: actual machine/nozzle/access/material/profile/ventilation/cost proof and separate print grant |
| INV-FIL-01 | Actual spool/ownership UNKNOWN | No owner-grade/material record supplied | Standard PLA; complete-set target 130–170 g, cap 170 g | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-POLICY-A D-12/D-14; SRC-PACK-A | A/V | G2/G3: actual spool/grade/condition/profile, slicing/waste ledger and heat/fit evidence |
| INV-CASE-01 | No physical case reported owned | No source geometry/fit proof | Original Jalak Gembul; nominal 150 mm, torso 112×102×122 mm; four pieces, clear reservation 70×55×80 mm | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | UNKNOWN | SRC-PACK-A | A/V | G1/G2/G3: actual envelopes/service constraints before CAD freeze; authorized geometry/print/fit/closure evidence later |

## Clearance, service and resolution ledger

| Item scope | Actual clearance/service facts | Required evidence / custodian role / blocked use |
|---|---|---|
| All electronic rows, including OLED | Connector projections, mounting/access, tool clearance, mass and condition UNKNOWN | Owner/authorized operator: sanitized labels and W×D×H/envelope in mm with method, uncertainty, condition, date/source under separate nonpowered-inspection grant; G1 fit/driver freeze BLOCKED |
| OLED | Glass boundary/active offset/bezel aperture/header depth UNKNOWN | Actual display evidence, not representative board outline; G1 BLOCKED |
| Mic/amp/speaker | Acoustic path, wiring compatibility, grille/opening and safe volume UNKNOWN | Authorized hardware custodian: actual part documents and later electrical/acoustic evidence; G1/G3 BLOCKED |
| SD/card/cell/charger/converter | Card insertion path, cell access/retention, cable/polarity/power-path clearance UNKNOWN | Specific part identity and service constraints; later approved safety inspection/test; G1/G3 BLOCKED |
| Case/control/supply/printer/filament | Simultaneous fit, service opening, control reach, rail safety, actual fabrication access UNKNOWN | Owner/authorized fabrication custodian: source/permission/profile/quote evidence before CAD/print/power; G1/G2/G3 BLOCKED |

No physical acquisition, inspection, voltage application, pin mapping, CAD or layout is performed by this documentary packet. Labels unresolved by future proof remain conflicts/UNKNOWN, not inferred alternatives. Every row feeds [safety resolution](01-SAFETY-BUDGET.md#power-and-cost-resolution-register) and [BAS coverage](01-VERIFICATION.md#expanded-requirement-and-gate-coverage); no BAS requirement is completed.
