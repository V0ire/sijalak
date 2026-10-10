# Phase 8: Iteration and Handover — Context

**Gathered:** 2026-10-09. **Status:** YOLO planning only; MAIN/user review pending; no interview or execution.
**Requirements:** OPS-01, OPS-02. Local D-19…D-24 extend, rather than redefine, D-01…D-18 in `.planning/DECISIONS.md`; the root register is not edited.

<domain>
## Phase boundary
After actual Phase 7 evidence is available, prepare a locally reviewable, operator-safe maintenance package and evidence-led continue/revise/stop recommendation. This phase consumes prior evidence; it neither reruns the pilot nor automatically deploys revisions.
</domain>

<decisions>
## Decisions
- **D-19 — Named authority before dispatch:** MAIN must obtain new explicit implementation approval for 08-01 before any executor workflow starts; the first task is a blocking-human decision rechecking authority. No automatic selection. **Rationale:** planning autonomy cannot grant operational permissions. **Refs:** `08-SPEC.md` AC-01; `.planning/VERIFICATION-STRATEGY.md` G0; `.planning/STATE.md` §Session Continuity.
- **D-20 — Actual evidence, legitimate stop:** consume the future Phase 7 report/verification or truthful blocked/stopped report; preserve comparison denominators, assistance, distractions, operator burden and limitations. Continue/revise/stop is an owner-reviewed recommendation, never automatic rollout. **Rationale:** robot appeal and D3 do not prove reading benefit; missing data is not favourable evidence. **Refs:** `.planning/ROADMAP.md` Phase 8; `.planning/DISCOVERY.md` §§6–7; `08-SPEC.md` AC-05…07.
- **D-21 — Local package and accountability:** select `docs/handover/{operator-guide,maintainer-guide,incident-runbook,evidence-review,revision-register}.md` as future Markdown outputs. Operator guide uses concise Bahasa Indonesia; maintainer/incident records name the supplied accountable roles, receipt/review state and escalation responsibility. **Rationale:** safe handover needs reachable instructions and ownership, not deployment automation or exposed credentials. **Refs:** `.planning/REQUIREMENTS.md` OPS-01; `.planning/ARCHITECTURE.md` §Responsibilities.
- **D-22 — Stop-first care and recovery:** document startup, STOP/reset, safe shutdown, cleaning/storage, failures and incidents against actual power/service evidence. No unverified charging, load-sharing or battery field operation; missing limits/procedure/maintainer means keep the unit withdrawn. **Rationale:** owned LiPo/modules and a shell cannot establish electrical or child safety. **Refs:** global D-03, D-15; `.planning/VERIFICATION-STRATEGY.md` §§Mechanical / print evidence, Reporting / exit.
- **D-23 — Versioned content/profile boundaries:** record actual firmware/backend/content compatibility, catalogue availability, human source/licence, disabling/update/rollback and privacy cleanup. Restricted adult-lab and child-local are separate deployment/build/permission paths; no user control enables adult AI in child-local. **Rationale:** updates must preserve isolation and truthful fallback, not silently expand a child service. **Refs:** global D-01, D-08, D-11, D-18; `.planning/AI-SPEC.md` §§2, 9; `.planning/ARCHITECTURE.md` §§1, 6.
- **D-24 — Honest as-built and cost records:** retain actual release/envelope/CAD-reference/profile revisions, material provenance, support/waste counts and heat/fit checks when available; original H150 four-piece PLA baseline remains a target until evidenced. Rates/FX/quotas must be actual or unknown; Rp100k/month does not approve spend and Rp500k extras exclude owned hardware/printer/API fees. **Rationale:** obsolete estimates and fabricated manufacturing/cost claims cannot support maintenance or rollout. **Refs:** global D-12…D-17; `.planning/INDUSTRIAL-DESIGN.md` §§3, 7, 9; `.planning/AI-SPEC.md` §8.

## the agent's Discretion
Document organisation, neutral metadata labels and cross-link wording may be selected. No discretion to invent persons/partners/observations, approve operations, weaken provider/safety gates, expand features or mark requirements implemented.
</decisions>

<canonical_refs>
## Canonical references — read before future work
### Scope, evidence and authority
- `.planning/PROJECT.md` — reading-start hypothesis, Pameungpeuk unknown baseline, provisional 10–14 cohort, permissions and services preserved.
- `.planning/DECISIONS.md` — U/D/A/V provenance and inherited D-01…D-18; `.planning/REQUIREMENTS.md` — OPS-01/02 and all-phase traceability.
- `.planning/ROADMAP.md` — Phase 7 → Phase 8 dependency and outcome; `.planning/MVP.md` — separate adult D3 and local literacy outputs.
- `.planning/DISCOVERY.md` §§3, 6–7 — voluntary participation, comparison/proxy and unknown effect criteria.
- `.planning/VERIFICATION-STRATEGY.md` — gates, proposed targets versus evidence and truthful reporting.
### Modes, care and maintenance
- `.planning/ARCHITECTURE.md` §§1, 4–8 — controls, profiles, cancellation, data, power and failures.
- `.planning/AI-SPEC.md` §§2, 4, 7–9 — provider eligibility, budgets/retention and evaluation before any expanded child AI.
- `.planning/INDUSTRIAL-DESIGN.md` §§3, 5, 7–10 — original hybrid, dimensions, four pieces, PLA consumption and revision/heat/fit gates; no invented wiring/mounting.
- `.planning/phases/08-handover/08-SPEC.md` — falsifiable acceptance and prohibitions.
### Conditional execution evidence
- Actual approved Phase 7 report/verification and linked prior-phase release/safety/content evidence: paths unresolved now, to be supplied by MAIN and recorded after named approval. Missing inputs halt dependent work; no guessed report paths.
</canonical_refs>

<code_context>
## Existing implementation context
No source, test, build, service, manufacturing or field result is inspected or generated by this planning task. The package describes verified prior implementations, not new code. Documentation-only discovery uses local canonical contracts; no new library, dependency lookup, provider contact or install is required.
</code_context>

<deferred>
## Deferred Ideas
RAG/web agents, visitor profiles, cloned voices, always-listening, reward/leaderboard loops, motors/cameras, detailed internal mounting/wiring, mass rollout and multi-unit dashboards remain excluded. Child live AI requires a newly eligible provider contract and threat/eval review, not a bypass. Credential rotation, deployments, prints, purchases and new fieldwork require separately scoped authorization/work; none is a task here.
</deferred>

## Permission boundary
External/contact, spend/billing, installation/build/testing, server/deployment, field, printing and Git permissions are not granted. Preserve global OpenCode/Pro/9Router and existing VPS services. Root state/roadmap/config updates remain MAIN's responsibility; current writes are only the three requested Phase 8 planning Markdown files.
