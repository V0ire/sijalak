# Industrial research: original companion enclosure

Research date / source access: **2026-10-09**. Public primary sources, retrieved through targeted Firecrawl search/scrape.
Evidence labels: **FACT** = source statement; **BRIEF** = user-provided information; **ASSUMPTION** = provisional engineering allowance; **INTERPRETATION** = design inspiration.
This is source research and a packaging brief. Geometry, simultaneous component fit, printing, support requirements, and assembled mass remain unverified.

## 1. Brief and known inventory

- **BRIEF:** original, round, chubby character; short legs, simple hands, small horns, playful expression. The written brief, not an attached image, defines the direction.
- **BRIEF:** size baseline interpreted for planning as total height **140–160 mm**; target **PLA consumption 130–170 g**. Count printed parts, supports, brim, and other print waste against filament consumption. Electronics/battery/fasteners are not PLA and must not be included in that target; assembled robot mass remains unknown. This height interpretation is provisional.
- **BRIEF:** owned ESP32-S3 N16R8, 1.3-inch 128×64 OLED, INMP441 microphone, MAX98357A amplifier, 3 W speaker, microSD, approximately 450 mAh / 3.7 V LiPo, charger/converter.
- **UNKNOWN:** exact manufacturers/variants, PCB outlines, connector protrusions, display glass/active-area offset, thicknesses, speaker size, cell dimensions, and individual masses.
- N16R8 describes memory configuration, not a development-board footprint. A 3 W speaker rating and 450 mAh battery capacity do not establish mechanical dimensions.

## 2. Flashforge Adventurer 5M: verified constraints

| Field | FACT / qualification | Evidence |
| --- | --- | --- |
| Build volume | **220×220×220 mm** for 5M; Pro also advertises this volume. | [F1], [F2] |
| Nozzles | 5M product page identifies **0.4 mm installed**; series guide lists **0.25, 0.6, 0.8 mm** optional hot ends. Actual owned nozzle/bundle must be checked. | [F1], [F3] |
| Temperature capability | Product advertises **280°C hot end**; series guide specifies bed heating **up to 110°C**. These are capabilities, not recommended PLA settings. | [F1], [F3] |
| Filament suitability | Series guide suggests PLA/PETG with **0.25/0.4 mm**, TPU/PLA-CF/PETG-CF with **0.6/0.8 mm**. Abrasive-material use also depends on the actual nozzle rating. | [F3] |
| Standard 5M | Official catalogue calls it **open-frame**. An optional DIY enclosure kit exists; ownership of that kit is not established. | [F5], [F6] |
| 5M Pro | **Enclosed**, with internal/external circulation and **HEPA + activated-carbon filtration**; additional auxiliary cooling is described in the series guide. | [F2], [F3] |
| Ventilation | 5M manual notice says to operate in a **well-ventilated environment**. Ordinary part cooling and an optional enclosure are not evidence of Pro-equivalent filtration. | [F3], [F4] |

- **Engineering implication:** PLA is the baseline; PETG is a feasible alternative with a validated material profile. Nozzle temperature alone does not establish open-frame ABS/ASA suitability.
- An enclosure or filter is not a demonstrated substitute for room ventilation; the manufacturer's filtration marketing is not a zero-emissions guarantee.
- A 140–160 mm height is below nominal Z capacity, but overall XY dimensions, part orientation, appendages, and adhesion margins still need checking.

## 3. Official FDM guidance and project allowances

| Topic | Manufacturer guidance / FACT | ASSUMPTION or consequence for this enclosure |
| --- | --- | --- |
| Walls | Prusa illustrates 0.4 mm nozzle / about 0.45 mm line width: 2/3/4 perimeters ≈ **0.90/1.35/1.80 mm**. These are approximations; line overlap and layer height change actual thickness. [P1], [P2] | Start with **1.8–2.2 mm** general PLA/PETG shell; **3–4 mm locally** at loaded feet, limb roots, or closure regions. These are project allowances, not official material-specific minimums. |
| PLA | Prusa describes good detail and low warping, but brittleness, weaker layer adhesion, and softening/deformation above roughly **60°C**. [P3] | Suitable for an indoor prototype; repeated flexing, drops, and heat can invalidate thin-shell assumptions. |
| PETG | Prusa describes toughness/good layer adhesion, but worse bridging/overhangs and harder support removal; more perimeters can improve solid-part strength. [P4] | Revalidate shell stiffness, finish, and fits; do not transfer PLA bridge performance unchanged. |
| Unsupported slopes | Prusa gives a general **45–60°** range dependent on nozzle/settings; its higher 75° claim is specifically for named newer Prusa machines. [P1] | Start conservatively at **≤45° outward overhang from vertical**: 0° vertical, 90° horizontal underside. This convention and limit are provisional, not an AD5M capability guarantee. |
| Bridges | Prusa says short horizontal bridges can print unsupported; it does not supply one universal maximum span. [P1] | Require existing material to anchor both ends and profile-specific validation. A broad cavity ceiling is not equivalent to a short tested bridge. |
| Orientation | Prusa recommends splitting parts to improve orientation; FFF strength is directional. Downward-facing fillets can overhang more severely than chamfers. [P1] | Choose orientation separately for shell, closure, hands, and horns; protect limb-root strength and visible face finish. |
| Fits | Prusa says there is no universal tolerance and suggests **at least 0.3 mm** as an initial movable-part allowance. Its printer accuracy claim is not AD5M evidence. [P1] | Trial removable slip fits at **0.25–0.40 mm clearance per mating side** (0.50–0.80 mm total across opposite sides), then calibrate. Snap/friction fits need separate trials. |

**Critical ceiling caveat:** a round exterior does not make a hollow interior self-supporting.
Large closed domes develop increasingly near-horizontal internal overhangs toward their crown; flat internal roofs may need to bridge the cavity width.
Openings for electronics do not automatically make trapped support removable. Splitting a dome into two pieces does not itself solve either problem.
Consider an open body plus separately oriented closure, a revised internal roof, or removable supports; validate every underside and closing layer later.
Flush/stubby hands and broad-rooted, tapered horns reduce difficult projections, but **no support-free guarantee is justified** without actual geometry and print validation.

## 4. Real references: inspiration only

| Reference | Primary-source anchor / FACT | INTERPRETATION to borrow without copying |
| --- | --- | --- |
| Kidrobot MUNNY | Kidrobot describes a customizable blank 3D art-toy canvas and multiple sizes. [D1] | Large rounded head/mass, simplified hands and short limbs, sparse surface detail; develop independent proportions and outline. |
| UglyDolls / Babo | Hasbro traces the family to a doodle, describes OX as soft/squishy with quirky personality, and explicitly lists **Babo**. The retrieved page is for OX, not a Babo dimensional record. [D2] | Doodle-like simplicity, friendly imperfection, restrained asymmetry and stubby extremities. Babo-specific geometry was not independently verified from a direct primary page. |
| EnergizeLab Eilik | Official page emphasizes facial expressions, touch responses, and playful personality. [D3] | Treat the screen as the expression focus; use a compact soft body and simple hands. Product hardware, dimensions, and internals are not design requirements. |
| Anki Vector | Current official Anki/DDL product page presents a playful companion and says its appearance is largely unchanged from the earlier generation. [D4] | Personality communicated through face/posture; its mechanical, tread-based identity is a useful contrast to the requested soft-legged creature, not a body template. |
| Jalak / Bali myna | San Diego Zoo describes white plumage, black wing/tail tips, blue skin around the eyes, and a feather crest. This is **Bali myna**, not every jalak species. [D5] | Optional eye-area accent, a restrained crest cue, tiny wing-like hands, or rounded breast gesture. Requested horns are fictional, not bird anatomy. |

| Direction | Brief fit | Engineering trade-off |
| --- | --- | --- |
| Rounded monster | Strong: horns, short legs, simple hands read naturally. | Keep silhouette original; avoid protruding horizontal hands and a sealed oversized hollow head. |
| Blob | Strong round/chubby quality, but weaker leg/hand/character cues. | Smooth closed domes create roof problems; low detail does not mean low material mass. |
| Original hybrid | **Preferred interpretation:** rounded monster/blob core, independent horn/limb proportions, optional subtle jalak cue. | Allows a flatter screen area and deliberate service seam while retaining a soft silhouette. |

Use these references for traits only. No reference assets/models were downloaded or copied into the project.

## 5. Representative component dimensions, not owned-part facts

| Official representative | Published dimensions | Limits |
| --- | --- | --- |
| Espressif ESP32-S3-DevKitC-1 v1.0 mechanical drawing | **62.74 mm long × 25.40 mm wide**. [C1] | Example development-board outline only; does not identify the owned N16R8 board or its assembled height. |
| Waveshare 1.3-inch OLED (A), SH1106, 128×64 | Board **40.5×37.5 mm**; display area **29.42×14.70 mm**. [C2] | Representative module, not the owned display. Header style, depth, glass outline, and active-area offset must be measured. |

A 1.3-inch diagonal is not a 1.3-inch-wide face window; the representative active area is small relative to a chubby 150 mm character.

## 6. Packaging and assembly reservations

All numbers below are **ASSUMPTIONS**, expressed **width × depth × height**; they are mechanical reservations, not an electronics layout.
- Keep the total-height baseline **140–160 mm**; begin near **150–160 mm** until actual parts are measured. Body width/depth remain unresolved.
- Reserve approximately **70×45×75 mm** of genuinely usable central interior (≈236 cm³), clear of wall thickness and curved unusable corners.
- Reserve a front screen region about **45×20×45 mm**, with a locally flat bezel/seat; final visible aperture follows the actual display, not the whole PCB.
- Reserve lower audio space about **50×40×30 mm**, with an outward grille and a distinct microphone opening. This is an allowance, not a claim that every 3 W speaker fits.
- Reconcile the three reservations, access paths, and connector projections together; their individual plausibility does not prove simultaneous fit in a rounded torso.
- Prefer an accessible front insert plus a removable **rear or bottom closure**; the opening must pass the largest serviced assembly, not just expose it.
- Preserve physical access to charging, microSD, and the battery; keep bottom access clear of the short feet and the resting surface.
- Measure actual thicknesses/protrusions and service-tool clearance before freezing the enclosure. Small-module names cannot validate closure geometry or heat clearance.

## 7. PLA consumption: target, not a slicing result

- **FACT:** the October 2021 Prusament PLA technical datasheet reports typical density **1.24 g/cm³**, ISO 1183. [M1]
- **ASSUMPTION:** use that value for preliminary PLA volume-to-mass conversion; the owned spool's grade/pigments/additives are unknown. It is not a universal certified PLA density.
- Installed PLA mass ≈ **1.24 × actual plastic volume in cm³**. External bounding volume and hollow air space are not plastic volume.
- **Analytical counterexample:** a fully closed 150 mm-diameter sphere with a uniform 2 mm wall contains about **137.6 cm³**, hence **170.7 g PLA** before electronics, limbs, or closures.
- The target height must therefore not be interpreted as a 140–160 mm solid/spherical body diameter. A smaller hollow torso, short feet/horns, and restrained local reinforcement are essential.
- **Illustrative filament budget only:** main shell **80–100 g** + printed crown/base/visor/appendages/reinforcement **35–55 g** + supports/brim/purge **5–15 g** ≈ **120–170 g**. These are assumptions, not measured/sliced allocations; target consumption remains 130–170 g and estimates below that range are acceptable if appearance/fit hold.
- Dome/roof thickness, reinforcement, solid feet/horns, grille, bezel, and seams can push printed plastic above the allowance. Assembled robot mass additionally includes electronics, cell, leads, and metal fasteners and must be measured separately.
- Removed supports, brim, and process waste count toward filament use but not installed mass. A slicer filament estimate must include those and still does not establish assembled robot weight.
- **No actual slicing exists.** Required later evidence: measured part dimensions/masses, reconciled cavity and service access, chosen wall/profile settings, support/orientation review, and weighed assembled prototype.

## 8. Primary-source URLs

All URLs accessed **2026-10-09**; targeted search excerpts were sufficient for some product/manual/TDS facts. Dates in source titles/paths identify older documents, not current publication dates.

[F1]: https://www.flashforge.com/products/adventurer-5m-3d-printer
[F2]: https://www.flashforge.com/products/adventurer-5m-pro-3d-printer
[F3]: https://www.flashforge.com/a/docs/adventurer-5m-series/introduction-to-adventurer-5m-series
[F4]: https://en.fss.flashforge.com/10000/software/b7d33c3374d4ac67fac5a50e9328c567.pdf
[F5]: https://www.flashforge.com/products/adventurer-5m-enclosure-kit
[F6]: https://www.flashforge.com/collections/3d-printers
[P1]: https://help.prusa3d.com/article/modeling-with-3d-printing-in-mind_164135
[P2]: https://help.prusa3d.com/article/layers-and-perimeters_1748
[P3]: https://help.prusa3d.com/article/pla_2062
[P4]: https://help.prusa3d.com/article/petg_2059
[D1]: https://www.kidrobot.com/collections/munny
[D2]: https://consumercare.hasbro.com/en-us/product/uglydolls-jokingly-yours-ox-stuffed-plush-toy-9-5-inches-tall/768ADA73-EAC1-4A76-BF9D-0033EC38511E
[D3]: https://energizelab.com/eilik
[D4]: https://anki.bot/products/vector-robot
[D5]: https://animals.sandiegozoo.org/animals/bali-myna
[C1]: https://dl.espressif.com/dl/DXF_ESP32-S3-DevKitC-1_V1_20210312CB.pdf
[C2]: https://www.waveshare.com/1.3inch-OLED-A.htm
[M1]: https://prusament.com/wp-content/uploads/2022/10/PLA_Prusament_TDS_2021_10_EN.pdf
