# Phase 2: TBM Discovery and Curation — Specification

**Created:** 2026-10-09. **Status:** planning baseline; execution not authorized.
**Requirements:** RSR-01, RSR-02, RSR-03 locked for planning, not empirical validation or implementation authority.
**Method:** autonomous choices from canonical sources; no interview or ambiguity score claimed.

## Goal
Owner/operator obtains permissioned evidence about reading starts and a small usable book catalog, as defined in `.planning/ROADMAP.md` Phase 2.

## Background
- U: Pameungpeuk, Kabupaten Bandung is the initial region; no TBM partner, permission, observed participant, visit or catalog is confirmed.
- U/A: independent visitor initiation with available operator support is a preference/hypothesis, not evidence of unsupervised usability or reading benefit.
- A: cohort 10–14 and short human-authored hook plus bounded choice are provisional. V: actual visitors, languages, access needs and barriers may change them.
- D: child-directed/likely-under-18 Gemini clients are prohibited; discovery/child-local content cannot use Gemini, capture audio or send participant speech off-device, including via parents/proxies/paid tiers.

## Requirements — Current / Target / Acceptance
1. **RSR-01 — Confirmed partner and voluntary participation.**
   - Current: a region and proposed procedures, without contact authorization, partner/access agreement or participant consent.
   - Target: separately authorized contacts; confirmed partner/custodian/site scope and accessible guardian-plus-child information, consent/assent, refusal and withdrawal procedures before observation/interviews.
   - Acceptance: each encounter traces to valid sanitized gate receipts; real identities/signatures/contact records remain off-repo; no data is collected before permitted notification/participation or after refusal/withdrawal.
2. **RSR-02 — Nonleading anonymous baseline evidence.**
   - Current: proposed questions and observation dimensions; zero actual observations/interviews.
   - Target: ordinary no-robot activity observations and short voluntary nonleading conversations, distinguishing [O] observed, [K] reported, [I] interpreted and [?] unknown.
   - Acceptance: actual evidence includes visit/activity context, reading intent, book access/selection, help, engagement, diversion, noise/queues/operator burden and disconfirming cases; no identifiers, photos, audio or invented encounters.
3. **RSR-03 — Evidence-selected cohort, proxy and catalog.**
   - Current: provisional 10–14 cohort, ≥30s content-engagement/assisted-reading proxy and no checked books.
   - Target: site-supported cohort/proxy decision, 6–12 actual available titles and a prepared matched no-robot versus robot concept comparison.
   - Acceptance: shelf-verified catalog entries and source-linked cohort/proxy rationale include assistance/accessibility and revisions; missing books or invalid proxies remain unresolved, not filled with fictional titles or literacy-impact claims.

## Boundaries
- In scope after new permissions: partner/procedure confirmation, baseline/interviews, shelf curation, evidence synthesis and comparison preparation; not an implemented robot pilot.
- Future outputs: `02-AUTHORIZATION.md`, `02-FIELD-PROTOCOL.md`, `02-BASELINE.md`, `02-BOOK-CATALOG.md`, `02-VERIFICATION.md`, `02-01-SUMMARY.md` in this phase directory; none is produced by this planning task.
- Phase dependency: accepted Phase 1 evidence/permission boundaries precede Phase 2 execution; no dependency on phases 4–6 demo completion. Plan wave 1/empty dependencies are phase-local scheduling only.
- Out of scope: provider/API/VPS access, firmware/CAD/printing, robot deployment, recordings/assets, comprehension diagnosis, efficacy study and pilot execution (phase 7).
- MAIN retains root documents/config and canonical revision decisions; this phase proposes evidence-backed changes without editing roots/global OpenCode/Pro/9Router.

## Constraints
- Separate G0 authorization names **02-01** before executor startup; contact, field collection/site participation and child procedures are additional gates, never bundled with drafting authority.
- A: suggested operator interview 10–15min, two 30–45min baseline periods, 4–6 varied voluntary 3–5min conversations; revise to actual attendance/access, not mandatory recruitment quotas.
- Do not force 10–14 if irrelevant; include picture-book/shared/assisted engagement without reading-ability diagnosis. Revise proxy with operator before interpreting pilot results.
- Human/original/licensed child content only; ownership of a book is not permission to copy its text/audio. Public bibliographic facts and rights references may enter catalog records, not participant identities.
- Access/custodian/retention/deletion and small-group re-identification safeguards must be agreed before collection; no persistent cross-visit identity tracking.
- Match books/cues/attention and invitation/exposure/opt-in denominators for comparisons; no comparison of all baseline visitors with only self-selected robot users.

## Acceptance Criteria — Future Evidence Only
- [ ] RSR-01 gates exist and precede all participant data, with voluntary refusal/withdrawal and guardian consent plus child willingness where applicable.
- [ ] RSR-02 notes are source-separated, nonleading, anonymized and free of recordings/identifying quotations; absent evidence stays absent.
- [ ] RSR-03 cohort/proxy and 6–12 available books are backed by actual site evidence or transparently marked blocked/revised.
- [ ] Comparison preparation includes invitation/choice/hook alternatives and card/relawan no-robot comparator, burden/harms and denominator definitions, without claiming impact from preferences.

## Edge Coverage
| Edge | Requirement | Planned handling / future evidence |
|------|-------------|------------------------------------|
| No partner/access, guardian absent, refusal or revoked assent | RSR-01 | Stop affected collection; record only nonidentifying permitted gate status; no replacement consent inferred. |
| Different visitor mix/language/access needs; low attendance | RSR-02/03 | Revise cohort/protocol scope through appropriate permissions; disclose opportunity counts and limits. |
| <6 real suitable books, withdrawn title, unclear content rights | RSR-03 | Re-curate verified holdings under approved scope; block ready catalog/hook use rather than invent entries. |
| Holding book/robot play confused with reading; selection bias | RSR-02/03 | Separate observable categories/help; validate proxy and matched denominators, not comprehension/causality. |

## Prohibitions
No contact/fieldwork now; no covert observation, coercion, participant diagnosis, identities/photos/audio, fabricated cases/catalogs or child Gemini/audio egress; no parent/proxy/paid bypass, deployed robot pilot or root/global edits.

## Assumptions and Future Gates
A: phase-local anonymous Markdown evidence and human/card concepts suffice for discovery. V: Phase 1 accepted readiness, G0 named-plan authority, separate contact authorization, confirmed partner/site procedures, guardian-plus-child participation, actual visitor/holdings/proxy evidence and later G5 deployment readiness remain unmet. These unknowns permit drafting, not execution or invented outcomes.
