# Phase 03 — Industrial Design: Canonical Research Synthesis

**Created:** 2026-10-09. **Scope:** IDN-01–IDN-08 across existing 03-01/03-02; supplemental Markdown only.
No new source fetch, geometry/CAD/STL/3MF/G-code, slice, coupon, print, measurement, code/test or live proof is generated. Existing public-document records and owner reports are not actual prototype evidence.
U = owner report; D = recorded public documentation; A = selected design/analytical assumption; V = future evidence required. Previous source-access dates belong to existing research, not this supplement.

## Existing Sources and Evidence Level
| Source URL | Level / recorded fact or inspiration | Limitation |
|------------|-------------------------------------|------------|
| https://www.flashforge.com/products/adventurer-5m-3d-printer and https://www.flashforge.com/a/docs/adventurer-5m-series/introduction-to-adventurer-5m-series | D: existing INDUSTRIAL-RESEARCH records 220³mm and 0.4mm installed-nozzle references. | Owner's printer/access/nozzle/profile remain V; capabilities are not actual PLA settings. |
| https://www.flashforge.com/products/adventurer-5m-pro-3d-printer and https://en.fss.flashforge.com/10000/software/b7d33c3374d4ac67fac5a50e9328c567.pdf | D: Pro enclosure/filtration differs from standard open-frame 5M; manufacturer ventilation notice recorded. | No owned enclosure/filter, room condition or safe-emission claim inferred. |
| https://help.prusa3d.com/article/modeling-with-3d-printing-in-mind_164135 and https://help.prusa3d.com/article/layers-and-perimeters_1748 | D: directional strength, orientation/overhang/bridge/fit and approximate perimeter guidance. | No universal clearance/span or AD5M support-free guarantee; actual geometry/profile calibration is V. |
| https://help.prusa3d.com/article/pla_2062 and https://help.prusa3d.com/article/petg_2059 | D: PLA detail/brittleness/heat and PETG adhesion/bridge/support differences. | Not a certified operating-temperature or current-spool property. |
| https://prusament.com/wp-content/uploads/2022/10/PLA_Prusament_TDS_2021_10_EN.pdf | D: representative density 1.24g/cm³ in the older datasheet. | A volume-to-mass estimate, not measured spool density or slicer/assembled mass. |
| https://dl.espressif.com/dl/DXF_ESP32-S3-DevKitC-1_V1_20210312CB.pdf and https://www.waveshare.com/1.3inch-OLED-A.htm | D: representative board 62.74×25.40mm, OLED board 40.5×37.5/active 29.42×14.70mm. | Not owned N16R8 footprint/controller, actual aperture or simultaneous fit. |
| https://www.kidrobot.com/collections/munny and https://consumercare.hasbro.com/en-us/product/uglydolls-jokingly-yours-ox-stuffed-plush-toy-9-5-inches-tall/768ADA73-EAC1-4A76-BF9D-0033EC38511E | D/A: simple proportions/friendly imperfection inspiration. | Hasbro page is OX/family/Babo mention, not Babo measurements; no copied assets/geometry. |
| https://energizelab.com/eilik and https://anki.bot/products/vector-robot and https://animals.sandiegozoo.org/animals/bali-myna | D/A: digital-face/personality and species-specific crest/wing cues. | Not a body template, literal horn anatomy or validated character appeal. |

## Canonical Decisions and Reconciled Analysis
- U: owned electronics/exact variants unknown; total-height baseline 140–160mm, PLA 130–170g, printer probably 5M and Rp500k additional-material ceiling are reports/constraints, not fit/access/spending evidence.
- D-12/13/20/21: compare rounded monster/blob/original Jalak Gembul, then future editable MainBody/Crown/ServiceBase/Visor; nominal H150 includes feet/crest, torso 112×102×122mm, one true OLED focal region and separate audio paths.
- A/V: central 70×55×80mm, front 45×20×45mm, audio 50×40×30mm and service aperture ≥75×60mm are clear-space requirements/candidates; actual largest-assembly/tool sweep, OLED aperture and simultaneous envelope fit remain pending.
- Current SPEC/CONTEXT selections supersede older research central 70×45×75 and wall allowances: D-22 selects 1.8mm shell/four proposed 0.45mm lines and local 2.4–3.0mm, all still A/V.
- D-23/24: ≤45° from VERTICAL/support ≤10g are goals; inspect every closing layer/bridge/anchor and removal route. Trial 0.25–0.40mm PER SIDE (0.50–0.80 opposing-side total) requires real coupons.
- D-14:130–170g target/cap170g is one complete case print-set including its support/brim/purge/waste, excluding electronics. Coupons/reprints are separately tracked cumulative development use/cost, not combined with final-set mass; extra500k cash cap still applies. Lower material use accepted if fit/quality hold.
- A: canonical torso-surrogate 84.93g plus assumed caps/features/waste ≈135–160g is analytical, not slice/scale proof; H150 is not a 150mm sphere diameter. D-26 lean changes must preserve payload/service/face/strength.
- D-15/16/25: actual fit/service/retention/readability/10° tilt/approved heat checks and revision-linked re-slicing gate release; proposed ≤40°C surface limit is not a current pass. Power, budget and print risks remain unresolved.

## Authorization and Measurement Gates
- MAIN needs Phase 1 actual inventory/budget/power-readiness evidence and NEW real named 03-01 grant before executor startup; 03-02 wave 2 additionally consumes 03-01 and needs its OWN new grant.
- D-19: FreeCAD installation/tool acquisition, printer access, coupon/full-case printing, purchases and powered checks each need distinct approvals; no model/export/machine send follows from planning or another plan's grant.
- Actual part/control/connector/tool envelopes, printer/spool/profile, quotes, printed coupons, full/cumulative PLA, physical fit/face/retention/COM/thermal evidence remain V. No detailed PCB mounting, coordinates or wiring is added.
- Existing plans supply declarative future inspection text, NOT runnable CLI/test commands. `03-VALIDATION.md` preserves that distinction and names missing runtime bindings as future approved work, without inventing tools.

## Canonical Handoff
Read `.planning/phases/03-industrial-design/03-SPEC.md`, `03-CONTEXT.md`, `03-01-PLAN.md`, `03-02-PLAN.md`; rationale/sources are `.planning/INDUSTRIAL-DESIGN.md`, `.planning/research/INDUSTRIAL-RESEARCH.md`, `.planning/DECISIONS.md`, `.planning/ARCHITECTURE.md` and `.planning/VERIFICATION-STRATEGY.md`.
All measurements, geometry, support/mass/aperture-fit and safety acceptance remain pending; no existing plan/root/config/global OpenCode/Pro/9Router changes or child-use/certification claim is made.
