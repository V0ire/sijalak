# User flows and feature/subfeature breakdown

Planning-only baseline2026-10-09. Selection is provisional; observations/implementation absent. API model output never replaces controller permissions. See `ARCHITECTURE.md` for canonical limits.

## Capability tree and scope
| Capability | Subfeatures | Adult technical demo | Child-local literacy pilot |
|------------|-------------|----------------------|---------------------------|
| Start voluntarily | Clear start control, ready cue, decline/leave | PTT8s max; explicit capture | Start/cycle curated option; no capture |
| Express character | Blink/idle, listening/processing/speaking/error/handoff posture | Show actual pipeline state | Respond to local events, then quiet |
| Voice interaction | Capture, STT, bounded dialog, TTS, playback | Real Gemini through VPS only after eligibility/access approval | Human-authored/recorded/licensed local audio; no Gemini |
| Book invitation | Small hook, interest choice, known-book cue | Marked demo catalog; no fake holdings | Available6–12 titles, optional2–3 curated categories |
| Attention handoff | End conversation, calm eyes, no repeated solicitation | End_session/handoff action | Give space to book, no timer-as-proof |
| Control/recovery | Stop, reset, repeat boundedly, unavailable content | Cancel job/clear state/label fallback | Repeat/stop/local fallback without cloud |
| Operator care | Correct profile/content, volume, power, cleaning, incident stop | Restricted adult-lab and backend limits | Signed child-local deployment, consent and local catalog |
| Evidence | Bench metrics, reading-start observations, burden | Latency/error/cost/real voice integration | Anonymous matched observations, not device inference |

Deferred: spontaneous presence activation, motors, reward/streaks, timer, broad recommendation RAG, free web search, accounts/voiceprints, permanent profiles, live child Gemini, cloned voice and mixed-language generation. Two clear physical controls are selected over gesture novelty; exact parts/positions require measured case and usability review.

## Flow A — restricted adult internal R&D demo
1. Operator reviews provider eligibility, paid-service/data settings, consent, correct adult build, power/safety and current budget. No approved setup → no live call.
2. Consenting adult in controlled setting sees READY and chooses PTT. Robot captures only while activated; indicator remains visible.
3. Release/8s cap ends capture; robot sends bounded WAV to authenticated VPS session and shows PROCESSING.
4. VPS performs STT→chat→policy validation→TTS. Device polls boundedly; receives matching turn/generation audio.
5. Robot SPEAKING with mic inactive. Stop always cancels local playback and invalidates results; do not automatically resume old turn.
6. Repeat up to4turns/180s or hand off to labelled demo book. Session ends; mic/state/context reset. Real integration requires robot microphone and speaker, not laptop-only playback.

## Flow B — child-local pilot, hypothesis not validated
1. Prior partner/guardian procedures and child willingness; operator confirms child-local build and network-egress/provider restrictions. Existing Gemini adult build is not merely put behind an age prompt.
2. Visitor initiates general START; robot plays a brief human-approved hook. Visitor may leave/refuse without penalty.
3. If useful, visitor chooses among bounded curated categories using simple control(s); exact interaction tested with cohort first. Do not force a choice or assume inability from a declined response.
4. Robot points to a book/physical shelf cue actually checked by operator. Unavailable content → generic ask-human/stop; no invented titles/racks.
5. Robot becomes quiet/book-handoff state; attention belongs to book. Human observer may record the agreed reading-start proxy/assistance anonymously, not infer it from a button.
6. STOP/leave ends; no points/leaderboard/guilt or stored identity. Independent initiation does not mean no supervision.

## Flow C — refusal/fault
- Decline/start cancelled: silent reset, no negative judgement.
- Empty speech: bounded repeat once, then safe end; no emotional/ability diagnosis.
- Outside-context/harmful/injected request: safe response or end, no tool actions.
- Offline/timeout/budget/TTS error: clearly local/error fallback, never announced as a successful AI answer.
- Any physical/thermal concern: stop and remove from participant access; operator investigates after authorization.

## Flow D — operator handover
Confirm firmware/deployment profile → verify current permitted content/volume/power → start activity → monitor only aggregate status → stop/clean/store. Updates retain content provenance and actual book availability. Secrets, provider routing, firmware profiles and deployment are maintainer actions, not visitor controls.

## Persona / physical signifiers
Identity: rounded original Jalak Gembul, curious, mildly mischievous, humor aimed at itself/situation, not literacy level/person/identity. No sarcasm that shames children. Adult AI replies <=35words; child scripts created by humans and reviewed with manager. Bahasa Indonesia baseline; Sundanese/local phrasing deferred pending actual need and support.

OLED is a state/eyes device, not a tiny paragraph screen. Start and stop differ in tactile/label cues; states distinguish mic activity from idle even in monochrome. Face/readability at proposed0.3–0.8m, indoors, must be physically verified. No graphic/animation/image/UI implementation was generated during planning.

## Validation questions answered autonomously as assumptions
Primary cohort10–14 is a provisional research focus, not a claim about Pameungpeuk visitors. One visitor per session; queue/operator assistance and noise recorded. Short story-hook plus choice is selected over endless chat for child-local pilot; compare against equally curated non-robot invitations. Revise if field evidence prefers another intervention or no robot.
