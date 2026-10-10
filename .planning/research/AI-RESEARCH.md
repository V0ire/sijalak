# AI / stack research evidence

Collected2026-10-09. Public documentation only. No VPS, owner endpoint, Gemini model, audio upload, credentials, billing or account inspection. Indexed documentation is not proof of account capability; future implementation must validate actual contract after authorization.

## Confirmed owner reports vs uncertainty
VPS/API available; STT/chat/TTS available; no operational companion; provider “all Gemini, presumably.” These are U-level reports, not independent evidence. Current model IDs, audio formats, host routes, auth, billing tier, locale, capacities and retention are unknown.

## Primary provider terms — live quote
URL https://ai.google.dev/gemini-api/terms ; issuerGoogle; page lastupdated2026-04-28; live fetch2026-10-09.
> You must be18 years of age or older to use the APIs. You also will not use the Services as part of a website, application, or other service (collectively, “API Clients”) that is directed towards or is likely to be accessed by individuals under the age of18.

This applies to the Gemini API Additional Terms reviewed, not an invented rule covering every offering named Gemini. Assume Developer API until actual contract identified. Restrict internal adult R&D, separate from child-directed rollout; contract/eligibility review still needed before calls. No proxy/paid/parent exception stated. Choose local-human-script child pilot rather than violate it.

Unpaid terms: content and responses used to improve/develop products; human review possible; do not submit sensitive/confidential/personal information. Paid terms: prompts/responses not used to improve products under those terms, but limited security/abuse logs and transient global storage may remain. No zero-retention or automatic remote-deletion claim.

## Model and format documentation — indexed official, not live model tests
| Source | Retrieved claim | Planning use / uncertainty |
|--------|-----------------|----------------------------|
| https://ai.google.dev/gemini-api/docs/changelog | Aug26,2026: `gemini-3.5-transcribe` and streaming variant GA | Candidate dedicated STT; choose nonstreaming, validate API/account support |
| https://ai.google.dev/gemini-api/docs/models/gemini-3.1-flash-lite | Model/audio transcription example uses `gemini-3.1-flash-lite` | Candidate economical text dialog; not a measured accuracy/latency result |
| https://ai.google.dev/gemini-api/docs/changelog | Sep22,2026: `gemini-3.8-flash-tts` and `gemini-3.8-flash-lite-tts` GA | Candidate efficient TTS; exact provider contract/price/locale still gate |
| https://ai.google.dev/gemini-api/docs/generate-content/speech-generation | SDK examples use prebuiltKore; unaryWAV and configurableformats; streaming16-bit little-endian24kmono chunks described | Normalize via VPS to canonicalWAVPCM24k; do not infer robot decodes arbitrary provider output |
| https://ai.google.dev/gemini-api/docs/generate-content/transcribe | Dedicated audio transcription flow | Keep model roles behind adapters; no speech-to-speech/full-duplex assumption |

The public live speech-generation fetch timed out, so the model/codec statements above are **Context7-indexed official excerpts**; revalidate live docs/current account before implementation. Model/version statements may change. Exact Indonesian voice/transcription support is pending conformance, not claimed tested. Live realtime alternative was considered but deferred to reduce session/stream/echo/state complexity.

## Stack evidence — live public docs
- ESP-IDF stableS3 I2S page https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/i2s.html returned titlev6.1. It documents two I2S peripherals, separateRX/TX channels/clocks/slots and DMA;24-bit configurations have alignment/clock requirements. Select6.1.xbaseline, actualpatch/pins/rawwordconversion after inventory. No wiring/API firmware used now.
- FastAPI https://fastapi.tiangolo.com/async/ advises async def for awaitable network libraries and proper handling of blocking calls. Select asyncPython gateway with codec work isolated; no claim of measured performance or deployed backend.
- Research is rationale for architecture, not evidence the owned mic/amp/controller match examples. Owned parts and SDK setup remain gate.

## Price/budget limitation
Context7 returned a former `gemini-3.1-flash-tts-preview` price example ($1textinput/$20audiooutput per1Mtokens), NOT a verified price for selected3.8 models. Do not use it for selected-model cost estimate. Current candidate prices/accountFX/quotas remain pending; cost formula and fail-closed admission policy in `AI-SPEC.md` complete provisional planning without fabricated quotes.

## Decisions derived, not field results
Choose bounded turn-based WAV16k input/WAV24k output, adult4turn/180s, <=35words, paid setting after authorization, prebuilt voice/no cloning, small explicit catalog/noRAG, no childGemini traffic, no raw-content logs. These are A-level assumptions. No factual claim that the region's readers want AI, these serviceIDs exist in the VPS, or any performance/safety criterion passed.
