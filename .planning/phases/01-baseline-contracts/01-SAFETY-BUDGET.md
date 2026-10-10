# 01 Safety and budget — revision 2 DOC-FIRST packet

**Scope:** Tasks 2–3 documentary power/cost/permission dispositions. **Readiness/G1: BLOCKED; G2/G3/G4/G5: BLOCKED; phase_goal: NOT_VERIFIED; integration_admission: false.** Documentary gates are not runtime enforcement or safety validation. Sources resolve in [01-VERIFICATION.md](01-VERIFICATION.md#source-register).

## OLED power, cost and fit disposition

| record | input/source | Actual fact status | Proposed treatment / rationale | Admission and resolution |
|---|---|---|---|---|
| SAFE-OLED-01 | INV-OLED-01 / CON-OLED-01; SRC-U04 + SRC-OLED-C2 | Owned report U; representative dimensions D; owner voltage/controller/protection/envelope/condition UNKNOWN | Actual envelope must constrain later design; no electrical or fit conclusion from public outline | G1 BLOCKED. Owner/operator supplies specific label/controller/envelope/method/uncertainty under separate inspection permission; no acquisition here |
| SAFE-POWER-01 | SRC-ARCH-A §7; SRC-POLICY-A D-15 | Actual regulated rails/current/thermal/protection/power-path UNKNOWN | Verified external power first; regulated 5 V/≥2 A is an A candidate, not proof of safe rails or permission to inject 5 V into logic | G3 BLOCKED. Power/charge/flash and nonpowered physical inspection NOT_GRANTED; battery/charging/load-sharing use closed |
| SAFE-MATERIAL-01 | SRC-U05 + SRC-POLICY-A D-16; SRC-PACK-A | Actual quotes/payment/fit/access UNKNOWN | Extra-material allocations below; owned electronics excluded from additional acquisitions, not assumed to have zero replacement cost | Spend/print/CAD NOT_GRANTED. Quote and scope evidence required before admission |
| SAFE-OPS-01 | CON-STT/CHAT/TTS-01; SRC-U08-U09 + SRC-POLICY-A D-17 + SRC-TERMS-D | Host/account/model/voice/rates/FX/billing/eligibility UNKNOWN | Proposed Rp100,000/month operating ceiling is unapproved and separate from hardware materials | G1/G4 BLOCKED; unknown price/account/access denies live calls; no presumed free tier |

## Proposed extra-material allocation — not quotes

| Category | A allocation (IDR) | Actual quote / source / quote date / inclusions | Purchase status |
|---|---:|---|---|
| Case | 250,000 | UNKNOWN; no supplier quote supplied | NOT_GRANTED |
| Controls, power and fasteners | 100,000 | UNKNOWN; no supplier quote supplied | NOT_GRANTED |
| Reserve | 150,000 | A reserve, not a priced item or quote | NOT_GRANTED |
| Allocation total | 500,000 | 250,000 + 100,000 + 150,000 = 500,000; matches U-05 additional ceiling | No spending authorized |

SRC-U05 is `../../DECISIONS.md` U-05; allocations SRC-POLICY-A D-16, both local 2026-10-09 records. Actual committed/paid totals UNKNOWN, not zero. Printer procurement/access, fieldwork and VPS/API operations are excluded and must be separately disclosed/budgeted/authorized, never hidden in this material sum. Quotes/date/tax/shipping/inclusions and cumulative costs remain future evidence.

## Enclosure and filament boundaries

SRC-PACK-A: nominal height 150 mm within reported 140–160 mm; torso 112×102×122 mm, central clear 70×55×80 mm, four case pieces. These are A design inputs, not simultaneous fit, heat or service proof. Research central 70×45×75 mm is a superseded A proposal.

SRC-POLICY-A D-14: PLA target 130–170 g, cap 170 g **PER complete case set**, counting that set's parts/supports/brim/purge/waste. Electronics/PCB/battery/metal fasteners are not PLA; assembled mass is UNKNOWN and separate. Development coupons/reprints require a separate cumulative material/cash ledger; associated cash counts toward the overall extra-material cap. No slicer, print, mass or support-free result exists here.

## Operation exclusions — reference, not a new grant

[01-AUTHORIZATION.md](01-AUTHORIZATION.md) AUTH-U-02 is MAIN-owned and unchanged. Documentary local-source use and read-only document checks are allowed. No expiry supplied; changed/revoked/beyond-scope work stops.

**NOT_GRANTED:** Git init/branch/worktree/commit/tag/push or any Git commands; stock init/execute CLI/resolvers/hooks/auto-chain; additional installs/dependencies/builds; application/firmware/source code/CAD/STL/G-code/image/assets/scripts-file generation; VPS/SSH/private endpoint/model/network calls; credentials/global configuration access or changes; billing/spending/purchases; printing; field contacts/participant collection; powering/charging/flashing or physical hardware inspection, including nonpowered inspection. Existing OpenCode, Pro, 9Router and plugins remain untouched. Roots, authorization, continuation and historical checkpoint are MAIN-owned.

Child-local no capture/no cloud and Gemini Developer audience restrictions from SRC-TERMS-D remain binding; adult-client eligibility is future proof, not an exception. These records do not authorize field activity or provider usage. [BAS tracer dispositions](01-VERIFICATION.md#source-to-readiness-tracer) remain blocked.

## Power and cost resolution register

Actual electrical facts were not provided by authorized local sources. Manufacturer protection/ratings are UNKNOWN, not assumed safe. The reported cell's nominal 450 mAh × 3.7 V ≈1.665 Wh is an A arithmetic illustration from SRC-POLICY-A D-15; it is not usable energy, runtime, current capability or safe charging proof. No electrical test occurred.

| Scope / linked inventory | Actual sourced facts and status | Evidence needed / custodian role | Gate and denied admission |
|---|---|---|---|
| INV-ESP-01 / INV-OLED-01 | Rail limits, USB path, reverse/backfeed behavior, GPIO limits, board/controller identity UNKNOWN | Owner/authorized hardware operator supplies labels plus applicable manufacturer board/module documents; later authorized rail tests | G1/G3 BLOCKED; no 5 V-to-logic assumption, driver/fit/power freeze |
| INV-MIC-01 / INV-AMP-01 / INV-SPK-01 | Mic/amp rails, gain, speaker impedance/current/volume/thermal compatibility UNKNOWN | Specific module/load documents and separately authorized electrical/acoustic tests | G1/G3 BLOCKED; 3 W rating is not safety, loudness or supply proof |
| INV-SD-MOD-01 / INV-SD-CARD-01 | Module rail/level conversion, card compatibility/capacity, removable access UNKNOWN | Actual reader/card records, service clearance and later storage tests | G1/G3 BLOCKED; module does not establish card capacity or safe rail |
| INV-CELL-01 | Cell chemistry/label/protection/connector/polarity/condition/current/temperature limits UNKNOWN | Actual cell manufacturer limits and separately scoped operator observations; no participant debugging | G1/G3 BLOCKED; battery field use, charging and load-sharing closed |
| INV-CHG-01 / INV-CONV-01 | Charger/converter identity, charge profile, protection/load-sharing/power-path, rails/current/efficiency/thermal limits UNKNOWN | Actual module manufacturer documents and authorized later power/thermal compatibility evidence | G1/G3 BLOCKED; reported ownership gives no power permission |
| INV-CTRL-01 / INV-SUPPLY-01 | Buttons/ratings/supply/cable/rails/current/protection UNKNOWN | Actual selected items/documents, quote and scoped safety evidence | G1/G3 BLOCKED; external regulated 5 V/≥2 A remains candidate only |
| INV-CASE-01 / INV-PRINT-01 / INV-FIL-01 | Actual fit/closure/stability/heat/supports, printer/nozzle/filament/access and production cost UNKNOWN | Authorized fabrication custodian: actual machine/material/access quote and later geometry/slice/fit/thermal evidence | G1/G2/G3 BLOCKED; print capability/reference dimensions do not prove safety or eligibility |
| CON-STT/CHAT/TTS-01 | Offering/account/retention/eligibility/rates/FX/quotas/billing UNKNOWN | Authorized service custodian supplies redacted genuine contract, dated rates/account settings and distinct billing/call permission | G1/G4 BLOCKED; adult internal-client review still required; no child Developer-API route |

## Material and operating ledger dispositions

This is a documentary allocation/unknown ledger, not a fabricated transaction history. Actual committed/paid money, quantities, supplier quotes, quote dates/revisions and taxes/shipping/inclusions remain UNKNOWN for all buckets; no purchase was made by this executor. Prior owner expenditure is not assumed zero. Source SRC-U05/SRC-POLICY-A D-16/SRC-MVP-A, local records dated 2026-10-09.

| Ledger | Proposed inclusion/cap | Actual record/source/date/revision | Authorization / next evidence |
|---|---|---|---|
| Additional case cash | ≤Rp250,000 allocation; final case production and associated development cash tracked cumulatively | UNKNOWN quotes/committed/paid; no supplier/date/revision | NOT_GRANTED; obtain genuine dated quote/inclusions, revised allocation if needed before spend |
| Additional controls/power/fasteners cash | ≤Rp100,000 allocation | UNKNOWN quotes/quantities/committed/paid | NOT_GRANTED; evidence includes cables/fasteners/tax/shipping or explicit exclusions |
| Reserve cash | Rp150,000 allocation, not a purchasable item or permission | Actual draw/committed/paid UNKNOWN | NOT_GRANTED; no silent over-cap expenditure |
| Cumulative additional-material cash | Allocation sum Rp500,000; actual admitted cumulative expenditure must stay ≤Rp500,000 | Actual sum UNKNOWN, not certified below cap | Owner verifies cumulative evidence and separate spend grant; simplify/requote if projected over cap |
| Existing electronics | Reported owned (U), excluded from additional acquisition allocation | Actual historical spend/value/replacement cost UNKNOWN | No claim of free replacements; future additional purchases need allocation and permission |
| Printer procurement/access | Separately excluded from Rp500,000; no assumed available/free printer | Actual access/quote/cost/source/date UNKNOWN | Separate disclosed budget/access/print grant needed |
| Fieldwork/participants | Separately excluded; not a cost hidden in material allocation | Actual site/cost/consent UNKNOWN | Separate field/participant permission; no contact/collection here |
| VPS/API operations | Proposed Rp100,000/month operating ceiling, separate and **unapproved** | Hosting/model rates/currency/FX/account/tier/usage/taxes/quotes/billing UNKNOWN | Unknown price denies admission; separate dated current pricing, FX/account review, usage caps and billing/call grant |

## Filament accounting and acceptance boundary

| Ledger | Proposed measure / limit (A) | Actual source/status | Future proof and admission |
|---|---|---|---|
| One complete final case print-set | All body/crown/base/visor parts plus supports/brim/purge/waste: target 130–170 g, cap 170 g; lower accepted if fit/quality hold | Actual slicer/weighed consumption UNKNOWN | Authorized slicing/print profile and waste-inclusive per-set total; G2 BLOCKED |
| Installed PLA and assembled mass | Installed plastic excludes removed waste; assembled mass separately includes electronics/cell/leads/metal | Actual masses UNKNOWN | Record separately, never equate PLA consumption to whole robot weight |
| Development coupons/reprints | Separate cumulative consumption ledger, not silently hidden in one final-set figure | Actual quantities/masses/cash/source/date UNKNOWN | Track each future authorized run; all associated extra-material cash within cumulative Rp500,000 |

Actual geometry, roof/bridge/support removal, service paths, sharp edges/retention/tip-over, volume/heat/stop behavior remain unverified. SRC-PRINT-D manufacturer references are not permission or empirical tests. No support-free, simultaneous-fit, electrical-safety or toy-certification claim is made. Signal to all downstream work: resolve relevant G1 evidence and specific scope **before** CAD/model/driver freeze; resolve G3 before powered use and G4 eligibility/pricing before model calls. No external setup is requested or executed now.
