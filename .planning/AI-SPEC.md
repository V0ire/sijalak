# AI specification — adult-lab D3 / local child pilot

Planning-only2026-10-09. No model/API called. Owner says all services “Gemini, presumably”; identity/account/contracts not tested. Roles are auto-selected with reversible adapter design, not evidence of endpoint readiness. Requirement AI-01…AI-06, phase4; API/FW/INT and pilot constraints cross-reference `ARCHITECTURE.md`.

## 1. Domain and real success
Literacy hypothesis: help visitors start reading. Robot attention, demo likability or speech accuracy does not establish that outcome. Technical target is actual robot mic→VPS→STT/chat/TTS→robot speaker for consenting adults; child field value is tested separately through human-curated local interactions.

## 2. Critical eligibility / data gate
Official Google **Gemini API Additional Terms**, last updated2026-04-28, live read2026-10-09: users must18+, and Services must not be used in API clients directed toward or likely accessed by under18s. Source https://ai.google.dev/gemini-api/terms.
- Assume Gemini **Developer API** until actual offering/contract is identified. Vertex/enterprise/other-provider contracts require their own review; don't claim an exception.
- D3 is a **separated, restricted adult internal R&D client**; provider eligibility review required before first live call. It is not a child-directed deployment behind a parent checkbox or an adult relaying children's speech.
- Child pilot profile has no live-Gemini credentials/routes/capture; no Gemini authoring/generation pipeline is presumed for child scripts. Human-authored/human-recorded or appropriately licensed content only.
- Paid tier/proxy/VPS/guardian consent does not waive age restriction. If adult-R&D eligibility is not confirmed, do not call Gemini; evaluate an eligible provider/contract after authorization, not secretly substitute.
- Free/unpaid services may use inputs/outputs for improvement and human review; terms prohibit sensitive/confidential/personal submissions. Prefer paid-service setting for eligible adult demo after approval; paid data is not used for product training under those terms, but security logging/transient global retention can remain.
- Local reset != provider deletion. No claim zero retention/anonymity without contract evidence.

## 3. Framework and model-role decisions
Gateway FastAPI/Python3.12; Google GenAI SDK behind provider adapters, exact dependency pin after authorized compatibility review. Firmware sees canonical audio/job contract, never Google-specific credentials/model payloads.
| Role | Provisional candidate | Evidence / uncertainty |
|------|-----------------------|------------------------|
| STT | `gemini-3.5-transcribe` | Official indexed changelog2026-08-26 reports GA dedicated nonstreaming transcription; owner's account/gateway support unknown |
| Dialog | `gemini-3.1-flash-lite` | Official indexed model/audio example; efficient bounded text role; no measured quality/latency |
| TTS | `gemini-3.8-flash-lite-tts` | Official indexed changelog2026-09-22 reports GA TTS; exact API/voice/language/prices must be checked |
| Voice | Prebuilt `Kore`, provisional | Official SDK examples use it; voice availability/Indonesian intelligibility pending |

Stable documented roles preferred over preview/Live streaming for first turn-based demo. If IDs aren't available, record reason and select a verified equivalent role through review; no unlogged fallback, no installed model/GPU assumed. No pricing for older preview model is applied to newer candidate.

## 4. Input / output / controller boundaries
- Bahasa Indonesia selected; Sundanese/code-switching deferred until provider support/field needs tested. Do not infer speaker identity/emotion/age/literacy ability from audio.
- Input explicit PTT, WAVPCM16/16kmono<=8s. Invalid/silent speech is bounded retry/end; no imagined transcript.
- Server session: opaque ID/device ownership, max4turns/180s, small curated context, TTL10min, no cross-visitor history. Child devices cannot obtain adult-lab permission by profile flag alone.
- Reply schema: reply_text<=35words; action only continue/handoff_book/safe_refusal/end_session; book_id nullable in allowlist; controller decides allowed behavior. No LLM-provided tool/GPIO/credentials/remote URL.
- TTS only validated allowed text; normalize provider audio to WAVPCM16/24kmono<=15s/768KiB. Zero ability/identity shaming, threats/guilt or false unavailable-book claims.
- Physical states expose mic/process/speech/error; mic inactive idle/speaking/stop/child profile. Backend and client reject stale/cancelled generations.

## 5. Persona and grounding
Curious/playful, slightly mischievous; sarcasm directed at robot/situation, never user/reading skill/identity. Responses brief and age-appropriate in local field mode. Internal adult AI persona is bounded, not permission to market Gemini client to children.
Adult demo catalog may use clearly labelled synthetic entries. Child catalog requires actual available6–12 titles verified with manager. Book/shelf facts must come only from active catalog; absent data means ask-human/uncertain, not fabricated recommendations.
No broad RAG/vector store, web search/agent tools, persistent profiles, voice cloning or emotional diagnosis in v0.1. Small explicit catalog and validator beat complex retrieval until evidence demands it.

## 6. Guardrails and negative cases
| Risk | Mitigation / evidence required |
|------|-------------------------------|
| Under18/child-directed provider violation | Separate builds/client deployment and backend permission; review contract first; child mode no Gemini egress |
| Unsafe persona/request/injection | System instructions + schema/allowlist/content validation + tests + human stop; prompt not sufficient |
| Hallucinated collection | Validated book_id/shelf data and synthetic-demo labels; fail closed when missing |
| Always-listening or cross-session leak | PTT, explicit state, buffers/context purge, ownership/generation checks; no raw-content logging |
| Stale audio / reset while request finishes | Invalidate generation, discard results and clear queue; backend cancel best effort, no promise provider cancels billing/deletes |
| Cost runaway / rate/loop failure | Input/word/turn/time bounds; app admission budget/rate cap; limited retry/idempotency; unconfigured rates block live calls |
| Provider/model lifecycle | Explicit role pins/contract snapshots; unsupported model produces honest failure, not fake recordedAI |
| Voice sounds unintelligible / codec mismatch | Fixed normalized format and robot-speaker test, not PC-only success |

Fallback local audio explicitly labelled error/local. Fallback isn't a successfulAI turn. This is planning, not evidence that any guardrail currently works.

## 7. Evaluation contract
Use synthetic prompts and consenting adults only after authorization; no real child speech or field data collected now. Start proposed>=30 voice turns plus deterministic negative fixtures; keep case IDs and rubric outcomes, not private raw content logs.
Case families: normal reading interests; synonyms/unseen utterances; silence/noise/too-long input; unknown book/shelf; off-topic; harmful or identity-shaming asks; prompt injection; invalid/long model output; cancelled/reset session; duplicate/unauthorized job; outage/timeout/budget exhausted; unsupported language/model; wrong audio format.
Rubric dimensions: intent fidelity, contextual usefulness, grounded references, brevity/persona restraint, safe handling, intelligibility, correct states/cancellation and truthful fallback. Assessments by maintainer/consenting-adult reviewers, not self-scored model alone.
Proposed engineering targets: >=90% clear-utterance intent, >=95% eligible successfulturns, p95 end-speech→first audible<=10s under a statednetwork across>=30turns. Measure stage+end-to-end latency, retry/error and cost; targets not measured results or population guarantees.
Hard negative fixture gates:0 cross-session/stale playback,0 unauthorized child/cloud calls,0 controller execution of invalid/harmful actions in chosen set. A zero-fixture failure count is not a universal content-safety guarantee.

## 8. Cost and observability
Hardware extra500k excludes VPS/API operations. Placeholder operating ceilingRp100k/month is not spending approval. Rates/FX/account quotas not verified; no token cost invented.
Cost estimate per session = STT billed units×current rate + dialog input/output units×rates + TTS units×rate + hosting attributable fees. Snapshot actual model/SDK rates and billed units; do not use3.1-preview price for3.8GA candidates. Maintain conservative pre-call allowance and usage reconciliation; provider budget alerts are not necessarily hard spend limits.
Default record metadata only: case/session/job opaque IDs, status, durations, model role/revision, counts and estimated/billed cost. No audio/transcript/reply/persona identifiers in logs. Backend transient audio expiry60s after completion and session TTL10min, local cleanup on stop; provider retention remains separate.

## 9. Preconditions / tests / monitoring — no current execution
Before live use: implementation+dependency+server+billing authorization, eligible adult client, actual API specs/versions/models/language, authentication/limits/prices, data-policy review, actual device codec/power/stop checks. Run labelled mocks first, then approved live adult E2E. Report failures explicitly.
Before child pilot: distinct local build, no capture/cloudAI routes or token privilege, actual partner and consent, human content/license/catalog, safety/print/fit evidence. Any future live child provider needs a separate eligible contract and threat/eval review.
Monitoring/handover: schema/safety errors, latency percentiles, cost denial, mode/ownership violations, stale/stop counts; operator kill/rollback. No deployed monitoring or service configured now.

## 10. Sources and evidence level
Read2026-10-09: https://ai.google.dev/gemini-api/terms (live terms); https://ai.google.dev/gemini-api/docs/changelog (indexed official excerpts:Aug26/Sep22); https://ai.google.dev/gemini-api/docs/models/gemini-3.1-flash-lite ; https://ai.google.dev/gemini-api/docs/generate-content/transcribe ; https://ai.google.dev/gemini-api/docs/generate-content/speech-generation . Public speech-page live scrape timed out; current role/codec excerpts came from Context7, not an account/API check. See `research/AI-RESEARCH.md`; validate snapshot before deployment.
