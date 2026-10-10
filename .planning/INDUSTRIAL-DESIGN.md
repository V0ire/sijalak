# Industrial Design, 3D Printable Enclosure & Character Design

Planning baseline2026-10-09. **No CAD, STL, G-code, render, AI-generated image, slice or physical prototype produced.** User supplied a written creature description, not an attached image. Dimensions/scores/material allocations are design assumptions, not measured fit/print/likability results. Evidence: `research/INDUSTRIAL-RESEARCH.md`; requirements IDN-01…IDN-08; roadmap phase3 with integration in6.

## 1. Three concepts and selection
| Concept | Aesthetic/identity | Printability/material | Interior/access | Stability/CAD complexity | Decision |
|---------|-------------------|-----------------------|-----------------|--------------------------|----------|
| A Rounded monster | Naturally matches chunky legs/hands/small horns; can look generic | Hollow torso economical; oversized spherical head/closed roof adds mass/support | Broad belly permits payload, but dome corners/roof deceive volume estimates | Broad feet; ellipsoid/fillets are simple, horn roots need care | Retain as comparator, not copy an existing monster |
| B Blob creature | Clean soft outline, fewer details; weakest hand/leg/name identity | Fewer features do not guarantee less material; sealed dome roof still problematic | Smooth bulb can reduce genuinely usable corners and awkwardly hide service seam | Low base stable if flattened; simple surfaces but low personality | Lean fallback only if original silhouette/face remain strong |
| C Original hybrid — **Jalak Gembul** | Rounded monster core, folded-wing-like stub hands, two unequal blunt horn/crest nubs, proud tiny-foot stance; subtle fictional jalak cue | Separate crown removes large sealed roof; restrained protrusions, flat face/service datums | Body envelope designed around a reserved box and independent face/audio zones | Flattened foot/base support; moderately parametric instead of complex organic sculpt | **Selected provisional main concept** |

Judgement, not user-test scores: C best balances identity, deliberate seams and room. A is simpler to recognize as monster but less specific; B may save feature material but risks becoming a faceless blob. Reject copied silhouettes or tiny finger/feather details. Name is working nickname; Si Jalak Bedas remains project name.

## 2. Real references and what is borrowed
- **Kidrobot MUNNY**: https://www.kidrobot.com/collections/munny — simplified proportions/art-toy readability, not exact head/body geometry.
- **UglyDolls family/OX/Babo mention**: Hasbro source in research — friendly imperfect symmetry and simple limbs; no claimed direct Babo dimension study.
- **Eilik**: https://energizelab.com/eilik — digital face as expression focus, not body template/firmware.
- **Vector**: https://anki.bot/products/vector-robot — readable state/personality contrast; no treads/mechanical limbs selected.
- **Bali myna**: https://animals.sandiegozoo.org/animals/bali-myna — crest/wing/eye-area traits of that species, not all jalak and not literal horns.

All are inspiration links only; no assets/models copied. Chosen outline must remain distinct after grayscale/silhouette comparison. No character appeal claimed validated.

## 3. Size and envelope baseline
All dimensions mm, **W×D×H** unless labelled. Choose **overall height150** within user's140–160 baseline, not a150mm-diameter sphere.
| Parameter | Nominal / allowance | Why / gate |
|-----------|---------------------|------------|
| Torso | 112×102×122 | Broad rounded capsule/belly; ellipsoid is only material-estimate surrogate, not prescribed internal geometry |
| Overall width | ~128–136 including stub hands | Hands project8–12/side, not long horizontal arms |
| Overall depth | ~102–110 | Face datum/closure may change; no layout fixed |
| Foot/crest contribution | ~14 feet +14 horn/crest beyond122torso =150 | Rounded tips and broad roots, no fragile long appendages |
| Central usable reservation | **70×55×80 clear** | CAD design requirement, not proof it fits an ellipsoid or all boards together |
| Front display reservation | ~45×20×45 | Representative board40.5×37.5 is not owned-part evidence; include glass/header/connector offsets |
| Lower audio reservation | ~50×40×30 | Actual speaker diameter/depth unknown; microphone opening kept distinct |
| Service opening candidate | >=75×60 clear | Must pass largest serviced assembly with tools/connector allowance; actual fit gate can revise |
| Base/support polygon target | ~90×65 | Hidden flattened base supports two visible short feet; COM unmeasured |
| OLED face zone | ~56×38 | Framing area, **not active screen size**; actual aperture measured, representative active~29.42×14.70 |

Do not add reservation volumes and claim packing. Curved unusable corners, closures, wall/flange, connector bends and access paths must be reconciled later. N16R8 identifies memory, not board footprint. Reference DevKitC-1 drawing62.74×25.40 does not identify owned variant.

Selected geometry should explicitly preserve the central clear reservation, even if belly departs from a perfect ellipsoid. No PCB coordinates, board mounts, wire routing or detailed electrical construction is specified now. Leave revision allowance and do not freeze shell before part measurements.

## 4. Character and OLED integration
- Single focal point: dark face region with the actual OLED eyes, framed by warm off-white/neutral PLA body. Optional muted-green/blue small crest accent, not mandatory extra spool.
- Broad blunt crest/horn pair, e.g10/14mm height, root diameter>=10mm and tip radius>=3mm as design targets. Gentle asymmetric height gives identity without balancing two heavy masses.
- Folded-wing-like stub hands are integrated and tapered with body ramps; no separate fingers/fragile tabs. Short broad feet and hidden base web avoid a top-heavy toy standing on tiny points.
- Face locally flat datum; recess screen slightly for protection. Do not use tint/extra glossy cover by default if it reduces OLED visibility. Active display is not enlarged by the mask; don't render fake56mm eyes.
- Eyes ready/listening/processing/speaking/error/handoff states described in `USER-FLOWS.md`; no pixels/animation images created now. Readability target0.3–0.8m indoors, especially0.5m, pending physical checks.
- Aim clean intentional silhouette, one face region, restrained details and concealed service seams. “Polished” means future surface/fit evidence, not a current pass.

## 5. Part separation and service access
1. **Main hollow torso**, open at crown/base: no large permanently closed dome roof. Integrated blunt limbs/feet; removable supports remain accessible.
2. **Crown cap**: hides opening in a character feature. Broad flat print datum; short tapered nubs built upward. Closing geometry uses printable supported/infilled cap rather than trapped unsupported roof; final profile checked in slicer.
3. **Bottom service cap**, rearward seam/access candidate: large tool-opened aperture, not just a tiny charging hatch. Feet/standing surface must not obstruct removal. Closure mechanisms conservative/captive, exact hardware after coupon test.
4. **Visor/bezel insert**: measured aperture, may use black sample filament; must be captive/secured from inside, not a loose decorative snap toy. Display-module mounting detail deferred.

Plan screw/tool closures over repeatedly flexed thin PLA snaps. Closure lips/local mechanical reinforcement are envelope/access features, not PCB-mount design. No loose detachable horns/hands/feet; small hardware must be secured/captive before child access. Access charging/SD/battery after safe power procedure, not while running/charging.

## 6. Filament and finishing
**Standard PLA initial selection**: detail/low warping, suitable for indoor development on open-frame5M. Avoid default silk/brittle decorative blends or relying on unnamedPLA+ properties. Filament/color/profile/access still unconfirmed.
**PETG alternate** only if measured impact/heat/service needs justify it: tougher/layer adhesion but stringing/overhang/support/fit changes. ABS/ASA not selected on an unverified open printer/ventilation setup.5M Pro enclosure/filter is not inferred for5M.
Minimum finishing: remove brim/local accessible support, deburr edges, clean face/seam; no acetone smoothing/painting as required process. Two-color parts can be separately printed if small black sample/quote fits budget; don't silently buy two full spools. Face texture comes from calibrated profile, not mandatory sanding/filler.

## 7. FDM profile, orientation and material estimate
Machine assumption: Adventurer5M220³, stock0.4mm nozzle. Confirm actual printer/nozzle/access before prints; printer purchase outside500k prototype cap.
- General shell target **1.8mm** =4×proposed0.45mm extrusion width; acceptable1.6–1.8 after calibration. Model wall/perimeter count linked, not arbitrary mismatch. Local roots/closure regions2.4–3.0mm; don't make all walls thick to hide weak geometry.
- Starting layers0.20mm body, optional0.16mm face/crown; calibration/manufacturer spool profile determines temperatures/speeds. No G-code or duration estimate created.
- Main body prints base rim down, open crown; revise shoulders before unsupported slopes exceed **45° from vertical**. This is conservative design goal, not guaranteed printer capability.
- Crown prints from its broad flat datum; base flat; visor face datum chosen for clean visible surface. Validate underside fillets, hand roots, crown closing layers, display opening upper edge and grille bridges individually.
- Prefer ramp/chamfer/arched short spans to downward fillets and wide flat roofs. Small localized accessible supports permitted; targetsupport<=10g, none trapped/invisible or on face focal surface. No zero-support promise.

**Analytical estimate, NOT CAD or slice:** ellipsoid torso112×102×122 at1.8mm wall gives plastic volume68.49cm³. At documented representative PLA density1.24, ~**84.93g** torso surrogate. Add assumed45–65g caps/features/reinforcement and5–10g support/brim/purge → **~135–160g total PLA**. Real openings/removeable cap shape/infill can differ; arithmetic is a budget envelope, not exact part volume.
User baseline target **130–170g PLA consumption per complete case print-set**, upper planning cap170g for that set's parts+support/brim/purge/waste. Track development coupons/failed/revision prints separately, not as part of final-set mass; their actual cash consumption still counts within the additional500k prototype budget. Electronics/cell/metal hardware do not count as PLA; assembled mass unknown. A closed150mm sphere2mm-wall alone is~171gPLA; don't confuse height with spherical body diameter.

### Lean alternate, not automatic shrink
Compare H~140–145, W~104–108, D~94–98 with shallower decorative crown/stubs and careful local reinforcement. Planning estimate~115–145gPLA, not a slice. Preserve measured payload/service/face/readability constraints; if the70×55×80 reservation/access no longer holds, reject rather than shrink electronics or claim fit. Optimizing excess solid features/seams/support comes before shrinking overall size. Baseline150 remains selected until fit/slice evidence.

## 8. Risk and mitigation
| Risk | Mitigation / later evidence |
|------|-----------------------------|
| Hollow dome closes unsupported | Open body + oriented/infilled crown; inspect every closing layer/bridge and support-removal path |
| Cavity volume adequate but modules don't fit | Measure actual outlines/protrusions, preserve reservations and service opening, future dummy/physical fit; no volume-only proof |
| PLA limb roots break / loose tiny parts | Integrated broad ramps, short blunt nubs, local walls, layer-orientation review and physical retention checks |
| Heavy top/tiny feet tip | Hidden broad support polygon, actual assembled COM and10° tilt test all directions; no arbitrary ballast before mass budget |
| OLED looks too small / glare | Actual active-window proportions, dark focal region and0.5m test; no fake enlargement or tinted cover by default |
| Audio blocked/feeds back | Independent screen/audio openings, lower grille/mic path reservation, half-duplex mic-muted playback and closed-case acoustic test |
| Heat/charging deforms plastic or harms users | Verified external-power demo first; no battery charging/field power until protection/thermal evidence; ventilation cannot certify safety |
| Costs >500k / printer not available | Get access/quote, use case cap250k or lean/requote; full spool/tooling/access cash differs from consumed-filament cost |
| Smooth exterior requires heavy finishing | Plan seam/part/datums/profile, avoid decorative micro-detail; sample coupons before full-body print |

## 9. Future parametric CAD → prototype → slicing → fit → revision
Use **FreeCAD parametric model** as planning choice; no installation/model now. Parameters: H/W/D, belly/shoulder profile stations, face angle/datum, measured display window/offset, clear volume, service opening, wall=line_width×perimeters, crown/base thickness, slipclearance, limb projection/root/tip radius, foot footprint, grille geometry.
1. After authorization, measure real parts and register envelope revision; establish manufacturing datums.
2. Build reserved-envelope/dummy solids and character outer form; preserve cavity/service constraints; no detailed board mount/wire layout at this scope.
3. Split torso/crown/base/visor, inspect unsupported undersides; produce an approved CAD candidate only after envelope review.
4. Print only fit/closure/window coupons first after actual printer/material/profile check; calibrate initial0.25–0.40mm **per-side** slip allowance, not confusing it with total diametral clearance.
5. Slice complete assembly, total parts+waste; analyze supports, closing roof, bed orientation and material<=170g. Nothing printed if access/support risks remain.
6. Print prototype after permission; measure fit, closure cycles, display visibility, audio, stability/thermal with qualified handling. No child debugging.
7. Revise parameters and re-slice; log reason/changed dimension/mass/profile. Do not hide geometry change behind measured obsolete estimate.

## 10. Acceptance and pre-print checklist — all pending
- [ ] Three forms/trade-offs and original concept recorded; no copied reference mesh/AI image.
- [ ] H140–160 nominal150 and overallXY/orientation fit verified actual bed; appendages/brim included.
- [ ] Owned component/module and connector/active-window dimensions recorded; central/front/audio reservations and simultaneous access validated before CAD freeze.
- [ ] No detailed PCB mounting/wiring added to this stage; largest servicedassembly passes opening; display/audiopath preserved.
- [ ] Nominal shell/perimeters/profile agree; limb roots/closures/tips checked; no sharp burrs/loose small parts or impossible support removal.
- [ ] Total slicer PLA parts+support+brim+purge <=170g; baseline130–170, lean less accepted only if appearance/fit/strength hold. Assembled mass separately weighed.
- [ ] Coupon fit/closure/openability and support/bridge handling verified before full print.
- [ ] Face state visible at0.5m, footprint stable on proposed10°tilt assembled, and power/heat/audio tests pass approved limits before participant use.
- [ ] Actual nozzle, filament, room ventilation, material settings, print access/quotes and allowed budget recorded; no printer/filament procurement assumed.

Current result is **planning evidence only**. CAD packing, aesthetics/printability, material grams, fit, stability, acoustics, heat and child safety remain pending validation.
