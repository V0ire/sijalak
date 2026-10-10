# Phase 03: Industrial Design and Printable Enclosure — Specification

**Date:** 2026-10-09. **Status:** planning baseline only; no implementation or validation performed.
**Dependency:** Phase 1 actual inventory, budget and safe-power evidence before future implementation; Phase 3 may then run alongside Phases 2/4.
**Provenance:** U = owner report/request; D = existing documentation; A = selected assumption; V = validation pending. Locked here means locked for planning, not physically validated.
**Current authority:** create this SPEC, CONTEXT and exactly two PLAN Markdown files only. All other artifact paths are future outputs, not instructions to create them now.

## Goal
Character/casing can be developed and checked against room, printability, material and assembly targets without overdesigning internals, as specified in `.planning/ROADMAP.md` Phase 3.

## Requirements — current, target and acceptance
| ID / plan | Current | Target after separately authorized implementation | Acceptance criterion — all pending |
|---|---|---|---|
| IDN-01 / 03-01 | Three textual concepts and inspiration sources; no model or appeal evidence. | Original Jalak Gembul with recorded silhouette/proportion trade-offs. | [ ] Three distinct forms compared with real links; independent geometry, no copied assets or product outline. |
| IDN-02 / 03-01 | Nominal dimensions/reservations are A, not measured packing. | Measured actual envelopes and service clearance constrain the parametric candidate before freeze. | [ ] Actual variants/protrusions measured; central/front/audio reservations reconciled simultaneously with removal paths, not by adding volumes. |
| IDN-03 / 03-01 | Owned OLED controller/outline and actual speaker dimensions unknown. | Measured OLED window and separate adaptable display, speaker and microphone paths. | [ ] Active area remains unobscured; face framing is not display enlargement; audio paths and module revision allowance documented. |
| IDN-04 / 03-01 | Four-piece service concept only. | Open main body, crown, bottom service cap and internally secured visor with captive/integrated decoration. | [ ] Largest serviced assembly/tool envelope passes; closures reopen without brittle exposed clips or loose small parts; PCB mounting/wiring absent. |
| IDN-05 / 03-02 | Analytical mass/profile assumptions; no slice or spool/access confirmation. | Calibrated profile/orientations and complete PLA-consumption accounting. | [ ] All parts, support, brim, purge and other case-related PLA counted; target 130–170 g, cap 170 g; electronics excluded, assembled mass separate. |
| IDN-06 / 03-02 | Roof, bridge, breakage, heat and tip-over hazards untested. | Explicit mitigations with authorized coupon, slice and physical evidence. | [ ] Closing layers/support removal, roots/tips/edges, 10° stability and approved heat checks pass; no support-free or toy-certification claim. |
| IDN-07 / 03-02 | Future CAD/print/revision sequence described, not executed. | Revision-linked inventory → CAD candidate → coupons → complete slice → prototype → fit → revision gates. | [ ] Each gate cites authorization/input revision/evidence; changes invalidate affected earlier results and require re-slicing before another print. |
| IDN-08 / 03-01 | Character/readability/finishing targets only. | One OLED focal region, blunt crest/horns, folded stub wings and short broad feet. | [ ] Original silhouette and low-finishing criteria recorded; later actual face states readable at 0.5 m without relying on color alone. |
**Current-stage acceptance:** four Markdown artifacts, all eight IDs covered, correct two-wave dependencies and dormant human gates; no requirement is marked implemented or physically accepted.

## Three forms and real-reference comparison
| Form | Traits to interpret, not copy | Trade-off / selection rationale |
|---|---|---|
| A — rounded monster | [MUNNY](https://www.kidrobot.com/collections/munny) simplified proportions; [UglyDolls OX/Babo family reference](https://consumercare.hasbro.com/en-us/product/uglydolls-jokingly-yours-ox-stuffed-plush-toy-9-5-inches-tall/768ADA73-EAC1-4A76-BF9D-0033EC38511E) restrained imperfection. | Strong horns/hands/feet fit, but generic identity and an oversized closed head risk mass/support. Hasbro page is OX, not a Babo dimensional study. |
| B — blob | [Eilik](https://energizelab.com/eilik) as a screen-expression reference, not a claimed blob template. | Sparse rounded form simplifies detail but weakens limb/name cues; fewer features do not prove less material or a printable dome. |
| C — original hybrid Jalak Gembul | [Bali myna](https://animals.sandiegozoo.org/animals/bali-myna) crest/wing cue; [Vector](https://anki.bot/products/vector-robot) face/personality contrast, not treads or mechanisms. | Selected planning assumption: chubby monster core, stub-wing hands, unequal blunt crest nubs, broad hidden support and flat face/service datums; original proportions rather than product geometry. |
Links reproduce existing research provenance; none was visited now. No art, meshes, images, renders or character-appeal results are generated or claimed.

## Boundaries and constraints
- **In scope:** character/envelope, assembly/service, display/audio access, parametric feature-tree specification, material/orientation strategy and revision space; future CAD/prototype evidence only after authorization.
- **Out of scope:** detailed electronics packing/layout, PCB mounts, component coordinates, wiring/GPIO, firmware/backend, participant work, child deployment and toy certification; integrated powered-demonstration acceptance belongs to Phase 6.
- **A dimensions, mm W×D×H:** owner total-height baseline 140–160, nominal H150 includes feet and tallest crest; torso 112×102×122; width approximately 128–136 including hands; depth approximately 102–110; approximate feet/crest contributions 14/14.
- **Design requirements, not packing proof:** central 70×55×80 clear; front 45×20×45; lower audio 50×40×30; bottom aperture at least 75×60 clear; candidate support polygon 90×65. Walls, curves, flanges, connectors and tools consume space.
- **Representative D only:** DevKitC-1 outline 62.74×25.40; OLED board 40.5×37.5, active 29.42×14.70; not owner dimensions/controller facts. Face zone approximately 56×38 does not enlarge pixels or glass.
- **Assembly A:** four case pieces; limbs/feet integral to body, crest nubs integral to crown, visor captive from inside; tool/screw closure preferred. Horn root diameter at least 10 and tip radius at least 3 are targets, not certified strength.
- **FDM A:** standard PLA, 1.8 mm shell tied to four proposed 0.45 mm lines; local reinforcement 2.4–3.0 mm subject to validation. AD5M 220³/0.4 mm/open-frame are reference assumptions; actual machine/access/nozzle/spool unknown, Pro enclosure/filter not inferred.
- **Print A:** body base-rim down/open crown; crown broad datum, base flat, visor visible-face datum reviewed. Start 0.20 mm layers, optional 0.16 face/crown; temperature/speed require actual spool calibration. Overhang goal at most 45° from VERTICAL; local support goal at most 10 g; every closing layer/bridge needs validation, never a zero-support promise.
- **Mass A, not slice:** torso shell84.93g at representative1.24g/cm³ plus assumed features/waste≈135–160g. One complete case set includes its supports/brim/purge against170g; development coupons/reprints tracked separately with all costs charged to500k. No cumulative-lifetime170g cap; assembled mass distinct and unmeasured.
- **Budget A:** Rp500k additional prototype ceiling allocated Rp250k case / Rp100k controls-power-fasteners / Rp150k reserve; not quotes or spending permission. Printer purchase excluded; actual access, tooling/spool cash and printing charges must not be hidden.
- **Lean conditional alternative:** H140–145, W104–108, D94–98 and analytical 115–145 g only if payload/service/face/strength hold; optimize excess features/support first, never shrink the required cavity blindly.

## Edge coverage — planned backstops, no execution evidence
| Edge | Requirement | Future check / failure disposition |
|---|---|---|
| Wrong variant, curved corners, connector/tool obstruction | IDN-02/03/04 | Measured envelope and removal-path review before freeze; missing/conflicting data blocks approval. |
| Hollow dome, downward fillets, unsupported closing roof | IDN-05/06 | Actual layer/span/anchor/support-removal review plus calibrated coupons; split cap alone is not proof. |
| Tolerance drift, broken limb root, loose visor/hardware | IDN-04/06/07 | Coupons at 0.25–0.40 mm PER SIDE (0.50–0.80 across opposing sides); repeated closure/retention checks before full-print approval. |
| Small OLED/glare, blocked speaker or microphone | IDN-03/08 | Actual window/service fit and future 0.5 m face/acoustic checks using separately approved fixtures; unavailable evidence stays blocked. |
| Top-heavy assembly, heat or unsafe power | IDN-06 | Actual assembled 10° tilt all directions; safe external supply and heat limits approved first; proposed surface ≤40°C or stricter applicable limit, no battery charging assumption. |
| Missing printer, excessive waste/cost, lean fit loss, stale revision | IDN-05/07 | Confirm access/profile/quotes; complete and cumulative consumption ledger; reject over-cap or payload loss and revalidate changed geometry/profile. |

## Prohibitions
- MUST NOT create code, CAD/FCStd, STL, G-code, renders/images, fixtures/tests or prints now; run builds/installs/commands, network/server/API/field calls, Git/hooks, purchases or root/global/config changes.
- MUST NOT copy reference geometry/assets, invent measurements/controller/coordinates/packing evidence, equate analytical grams to slicing, claim zero supports or certify child/toy/electrical safety.
- MUST NOT treat YOLO planning, Phase 1 completion or approval of one plan as approval of another plan, installation, printer access, printing, purchases or powered checks.

## Assumptions and revision triggers
Actual modules/controller/control envelopes, connector/tool needs, printer/access/nozzle/spool, quotes, support behavior, fit, COM, heat/acoustics and perception remain unknown/V. Measured conflict requires an explicit recorded revision, not invented compatibility.
The current selected 70×55×80 reservation and 1.8 mm shell / 2.4–3.0 local allowance supersede earlier research allowances of 70×45×75 and 1.8–2.2 / 3–4; neither set is measured evidence. Canonical sources and numbered decisions are in `03-CONTEXT.md`.
No interview, ambiguity score, automated check or physical validation was performed; all future acceptance checkboxes remain unchecked.
