# Si Jalak Bedas — Decision Register

Date: 2026-10-09. YOLO **planning only**. Decisions below authorize no implementation, purchases, API calls, fieldwork, installs, deployment, Git, or global changes.

## Evidence labels
- **U**: reported/requested by owner; not independently tested.
- **D**: public documentation; not a claim about the owner's equipment/account.
- **A**: rational planning assumption, auto-selected and revisable.
- **V**: pending validation; required evidence/gate is named.

## User decisions preserved
| ID | Decision | Evidence / boundary |
|----|----------|---------------------|
| U-01 | Pameungpeuk, Kabupaten Bandung is initial region | No specific TBM or partnership confirmed |
| U-02 | Help visitors start reading is temporary intervention hypothesis | No observed root cause or impact |
| U-03 | Visitors initiate themselves; operator available | Preference, not verified usability or unsupervised use |
| U-04 | Electronics in original brief owned | Exact variants, condition, compatibility unknown |
| U-05 | Additional first-prototype materials/power/case ceiling Rp500.000 | Excludes owned parts, printer purchase, fieldwork, hosting/API fees; no spending permission |
| U-06 | First milestone: functional D3 flexible AI voice demo | Not a literacy pilot or validated product |
| U-07 | Robot uses VPS/API, no operational companion device | Development computer/admin tools are not a required conversation companion |
| U-08 | VPS/API and STT/chat/TTS reported available | Endpoint/schema/account/language/quotas not inspected |
| U-09 | “All Gemini, presumably” | Provider identification is tentative, not verified provisioning |
| U-10 | 140–160 mm and roughly 130–170 g PLA baseline | Interpret size as total height; PLA use includes supports/brim, excludes electronics |
| U-11 | Printer planned: Adventurer 5-series, probably 5M | Actual printer/nozzle/filament/access not confirmed |
| U-12 | Full remaining planning may proceed without questions | Implementation and external authorization still separate |

## Auto-selected planning decisions
| ID | Selection | Alternatives and rationale | Validation / revision trigger |
|----|-----------|----------------------------|-------------------------------|
| D-01 | Separate internal adult AI demo and local child pilot | Gemini Developer API terms restrict API clients directed to/likely accessed by under-18s. Proxy, paid tier, parent consent or age checkbox is not an exception. Preserve D3 in restricted adult R&D; no Gemini route for child product | Provider/account/contract review before any live use; alternate child-eligible provider requires new evidence |
| D-02 | Primary provisional field cohort 10–14; adult demo 18+ | Younger readers need more assisted-reading design; broad age range confounds first pilot. 10–14 gives a focused independence hypothesis, not observed demographics | TBM visitor mix, ability, consent and accessibility observations; revise if irrelevant |
| D-03 | Turn-taking, push-to-talk plus separate stop control | Always-listening/full-duplex increases privacy, echo, buffering, latency and troubleshooting. Flexible language does not require continuous listening | Button reach/usability, acoustic bench tests; do not add presence/gesture sensors by default |
| D-04 | Input WAV PCM16 mono 16 kHz, max 8 s; output normalized WAV PCM16 mono 24 kHz, max 15 s | Bounded, easy-to-test formats; normalize provider output on VPS. Raw mic format is not assumed identical | Existing API contract and device codec tests; change adapter rather than silently misinterpret samples |
| D-05 | Async FastAPI/Python 3.12 gateway in isolated namespace | Python fits speech/Google SDK processing; async network operations with blocking work isolated. Do not replace existing VPS services/9Router | API inventory may permit thin adapter rather than new service; exact packages pinned after authorization |
| D-06 | REST jobs with bounded polling/audio download; no WebSocket MVP | Sequential stages suit inspectable failures, cancellation and payload bounds. Streaming can be introduced only after measured latency warrants it | 10 s p95 first-audio target under declared test network; target not proven |
| D-07 | ESP-IDF 6.1.x as firmware baseline | Official stable S3 docs show separate I2S RX/TX; direct control of buffering/tasks. Arduino may be faster initially but increases adapter uncertainty | Pin patch/toolchain and controller driver after inventory; no SDK installed now |
| D-08 | Gemini role adapters, not model logic embedded in firmware | Candidates: 3.5 Transcribe, 3.1 Flash-Lite chat, 3.8 Flash-Lite TTS; official indexed docs, not account availability | Verify exact supported IDs/API contract and pricing before paid calls; no silent provider/model substitution |
| D-09 | Bahasa Indonesia, prebuilt voice initially | Code-switching/Sundanese adds unverified ASR/TTS complexity. No voice cloning or custom persistent voice creation | Provider language/voice tests and field language needs |
| D-10 | Adult sessions max 4 turns / 180 s, replies <=35 words | Prevent endless robot attention, cost escalation and long TTS payloads | Usability and reading-handoff observations; pending benchmarks |
| D-11 | Child-local intervention: short human-authored book hook plus bounded curated choice | Compare pure invitation, recommendation and story hook; choose hook+small choice as testable value hypothesis. No live AI, microphone capture or points leaderboard | Curated books actually available and observed barriers; discard/revise if no added benefit over human/card invitation |
| D-12 | Standard PLA, original rounded hybrid “Jalak Gembul” | Rounded monster strongly matches brief but generic; blob is simple but weak identity/interior roofs. Hybrid ties stub wings/crest to name and allows flat face/service seams | Industrial design/fit/print review; name and appearance remain provisional |
| D-13 | Nominal height150, torso112×102×122 mm; 4 case pieces | Hollow body open crown/base avoids sealed dome; separate crown, service base and visor. Integrated blunt limbs, no tiny detachable horns | Actual component envelopes, underside/support inspection, slicing and physical fit; no PCB layout/mounting design now |
| D-14 | One complete case print-set PLA cap170g, baseline target130–170; lower use accepted if fit/quality hold | Count that set's parts/support/brim/purge. Development coupons/reprints have a separate cumulative ledger; all associated cash remains within extra500k | Slicer per-set totals + cumulative development cost; no actual slicing today |
| D-15 | External regulated USB bench/demo power initially | 450 mAh gives about1.665 Wh nominal, not a runtime guarantee. Avoid unverified charger/boost/power-path behavior | Actual supply/rails/thermal checks; battery field use remains gated |
| D-16 | Budget caps: case250k, controls/power/fasteners100k, reserve150k | Allocation equals500k, not market quotes. Printer/access costs must not be hidden; no parts ordered | Obtain quotes and inventory; if over cap, simplify/requote, not silently spend |
| D-17 | Operating budget placeholder Rp100k/month, no calls until priced and approved | Hardware ceiling does not fund VPS/API. FX and current model rates unknown; app budget gate fails closed | Billing tier, current rates, FX, usage caps and account authorization |
| D-18 | Two signed/deployment firmware profiles: adult-lab and child-local | Child profile cannot acquire/enable Gemini route through user controls. Separate backend permissions; no mere age toggle | Build inspection and network-egress checks before child deployment; provider legal gate remains |

## Strong revision rationale
The child live-Gemini interpretation was revised because of **documented provider restrictions**, not fabricated field evidence. AI demo remains; AI in the child-facing product is pending provider eligibility. Planning scope selection is not proof of literacy impact.

## Sources and pending facts
Gemini terms: https://ai.google.dev/gemini-api/terms (live read, last updated2026-04-28; read2026-10-09). Models/audio/TTS via official documentation indexed by Context7; exact account support untested. Printer/FDM/reference sources: `research/INDUSTRIAL-RESEARCH.md`. Detail: `AI-SPEC.md`, `INDUSTRIAL-DESIGN.md`, `ARCHITECTURE.md`.
