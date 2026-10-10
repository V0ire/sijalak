# Si Jalak Bedas — backend gateway (offline-testable core)

A small Python gateway for the adult-lab voice demo. **This session implements and
tests only the credential-free core**: session/auth/limits/idempotency, response
schema validation, and cost admission. The real STT/chat/TTS providers are behind
an interface with a **stub** implementation.

## What is real vs. not

| Component | Status |
|-----------|--------|
| Session store, ownership, idempotency, rate/limit admission | implemented + unit-tested (simulated) |
| Reply schema + guardrail validation | implemented + unit-tested |
| Cost admission (fail-closed on unknown rates) | implemented + unit-tested |
| Provider adapters | interface + **stub** only (simulated) |
| Real Gemini/network calls | **not implemented here** — needs separate authorization |
| HTTP server | thin, optional; core is provider- and framework-agnostic |

## Run the tests

```
cd backend
python3 -m pytest -q
```

No API keys, no network, no credentials required.
