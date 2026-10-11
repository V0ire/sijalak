#pragma once
// Device API client contract.
//
// This is the *logic* half of SW-FW-05: how the device frames a turn, what it
// sends, how it interprets the response, and how it reacts to failures. It is
// deliberately transport-agnostic — it never touches WiFi/TLS/HTTP itself.
//
// The real transport (ESP-IDF esp_http_client over TLS) implements
// `HttpTransport`. Host tests use `jalak::mock::MockTransport`, which replays
// canned responses with no socket. Therefore: client logic is `simulated`,
// the real network path stays `unverified`.
//
// Contract (proposed, mirrors .planning/ARCHITECTURE.md and the backend):
//   POST /v1/turn            multipart WAV + JSON context; idempotency key
//   GET  /v1/turn/{id}       status poll (capped separately from new turns)
//   GET  /v1/turn/{id}/audio returns canonical WAV, <= output cap
//
// Nothing here invents a hostname, schema, or model. Those remain unknowns that
// MUST be filled from real provider inspection before any live call.

#include <atomic>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace jalak {

// ---------------------------------------------------------------------------
// Request / response value types (transport-independent).
// ---------------------------------------------------------------------------

struct TurnRequest {
    std::string session_id;
    std::string idempotency_key;  // client-generated; server dedupes on this
    std::string schema_version = "1";
    std::string content_type = "audio/wav";
    std::vector<std::uint8_t> wav_bytes;
    // Optional device-side context; kept small and explicit, never free-form.
    std::string locale = "id-ID";
};

enum class TurnStatus {
    Accepted,
    Processing,
    Done,
    Failed,
    Cancelled,
    Unknown,
};

const char* to_string(TurnStatus s);

struct TurnResponse {
    int http_status = 0;
    std::string turn_id;
    TurnStatus status = TurnStatus::Unknown;
    std::string reply_text;
    std::string action;              // continue|handoff_book|safe_refusal|end_session
    std::optional<std::string> book_id;
    std::string schema_version = "1";
    // Bytes the server says the audio will be; used for a pre-download cap check.
    std::size_t audio_bytes = 0;
};

// ---------------------------------------------------------------------------
// Transport interface. The real implementation is the only network code; it is
// NOT provided here. Returning std::nullopt / raising is represented by the
// boolean ok flags so the client logic can react without exceptions.
// ---------------------------------------------------------------------------

struct HttpResult {
    bool ok = false;          // transport succeeded (a response was received)
    int status = 0;           // HTTP status, 0 if no response
    std::string body;         // raw body; parsed by the client logic shim
    std::string error;        // transport-level error message, if any
    bool timed_out = false;   // transport gave up on its own deadline
    bool aborted = false;     // transport observed a cancel request
};

// Cooperative cancellation token. The STOP button (or a session close) flips
// this flag; in-flight client calls observe it between attempts and stop.
// This is the portable, testable half of cancellation; the real transport must
// also honor it inside a blocking read (unverified for the real driver).
class CancellationToken {
public:
    void request_abort() { aborted_.store(true, std::memory_order_relaxed); }
    bool aborted() const { return aborted_.load(std::memory_order_relaxed); }
    void reset() { aborted_.store(false, std::memory_order_relaxed); }

private:
    std::atomic<bool> aborted_{false};
};

// Monotonic-ish logical clock. Tests inject a controllable clock so timeout
// behavior is deterministic and needs no wall time.
class Clock {
public:
    virtual ~Clock() = default;
    virtual long long now_ms() = 0;
};

class HttpTransport {
public:
    virtual ~HttpTransport() = default;
    // POST JSON+base64(metadata) and raw WAV body. Body framing is the
    // transport's business; the client passes already-framed bytes.
    virtual HttpResult post_turn(const TurnRequest& req) = 0;
    // Status poll.
    virtual HttpResult get_turn(const std::string& turn_id) = 0;
    // Download the reply audio; must enforce the download cap.
    virtual HttpResult get_audio(const std::string& turn_id) = 0;
};

// ---------------------------------------------------------------------------
// Parsing / validation. Server output is UNTRUSTED. We parse defensively and
// reject anything malformed rather than guessing defaults.
// ---------------------------------------------------------------------------

struct ParseError {
    std::string reason;
};

// Minimal JSON field extraction (no external JSON lib on-device). Returns the
// string value for a top-level key, or std::nullopt. This is intentionally
// strict: it does not accept nested objects or arrays for the fields we read.
std::optional<std::string> json_get_string(const std::string& json, const std::string& key);

// Standard base64 encoding (RFC 4648, no line breaks). Portable and unit-tested;
// used to frame the WAV body for POST /v1/turn (the backend expects wav_base64).
std::string base64_encode(const std::uint8_t* data, std::size_t len);
std::string base64_encode(const std::vector<std::uint8_t>& bytes);

// Build the exact JSON body for POST /v1/turn that the backend accepts:
//   {"session_id","idempotency_key","wav_base64"}
// Kept as portable logic so host tests can assert the wire shape without a board.
std::string build_turn_request_body(const std::string& session_id,
                                    const std::string& idempotency_key,
                                    const std::vector<std::uint8_t>& wav_bytes);

// Parse the {"session_id":"..."} response from POST /v1/session.
// Returns nullopt if the field is absent/empty or the body is malformed.
std::optional<std::string> parse_session_id(const std::string& body);

// Turn the raw HTTP body of a POST /v1/turn into a validated TurnResponse, or an
// error. Applies the reply word limit and the action allowlist, mirroring
// backend reply_schema.py so both ends agree on one contract.
struct ParseOutcome {
    bool ok = false;
    TurnResponse response;
    ParseError error;
    // Classification of a failure, so the caller can decide whether a retry is
    // safe. `Ambiguous` means the server MAY have executed the turn (e.g. the
    // transport timed out after sending); the caller must NOT blindly retry and
    // should reconcile via poll_status using the idempotency key.
    enum class Kind {
        Ok,
        Validation,  // pre-flight local rejection; nothing was sent
        Cancelled,   // client-side cancellation; nothing sent or loop stopped
        Timeout,     // deadline/transport timeout; possibly ambiguous
        Transport,   // connection failed before a response; safe-ish to retry
    };
    Kind kind = Kind::Ok;

    bool ambiguous() const { return kind == Kind::Timeout; }
};

ParseOutcome parse_turn_response(const std::string& body, int http_status,
                                 int max_reply_words = 35);

// Allowlist-aware variant. `book_allowlist` must contain every book_id a reply
// is permitted to reference; anything else (or a handoff with no id) is
// rejected. This mirrors backend reply_schema.validate_reply so the two ends
// cannot diverge. Prefer this overload everywhere; the one above uses an empty
// allowlist, which rejects any book_id.
ParseOutcome parse_turn_response_with_allowlist(
    const std::string& body, int http_status,
    const std::vector<std::string>& book_allowlist, int max_reply_words = 35);

// Parse a status-poll body (turn_id + status only).
ParseOutcome parse_status_response(const std::string& body, int http_status);

// ---------------------------------------------------------------------------
// Client policy: retries, backoff, and caps. Pure functions so tests can pin
// the arithmetic without a clock or a socket.
// ---------------------------------------------------------------------------

struct RetryPolicy {
    int max_attempts = 3;             // including the first
    int base_backoff_ms = 250;
    int max_backoff_ms = 4000;
    // Retry only on transport failure or 5xx/429; never on 4xx (except 429).
    bool retry_on_status(int http_status) const;
    bool retry_on_transport_error(bool transport_ok) const {
        return !transport_ok;
    }
    // Deterministic full-jitter-free exponential backoff for attempt n (1-based).
    // Determinism is deliberate: firmware tests must not be flaky.
    int backoff_ms(int attempt) const;
};

// ---------------------------------------------------------------------------
// Device API client. Holds no credentials and performs no I/O beyond the
// injected transport. This is the single place turn/status/audio policy lives.
// ---------------------------------------------------------------------------

class DeviceApiClient {
public:
    DeviceApiClient(HttpTransport& transport, RetryPolicy policy = {});

    // Optional cooperative cancellation + logical clock. When set, in-flight
    // calls check the token between attempts and stop immediately, and a
    // per-request deadline (set via set_deadline_ms) aborts the retry loop once
    // the clock passes it. Both are portable and unit-tested; the real transport
    // must additionally honor them inside a blocking read (unverified).
    void set_cancellation(CancellationToken* token) { cancel_ = token; }
    void set_clock(Clock* clock) { clock_ = clock; }
    // Deadline measured by the injected clock. 0 disables the deadline.
    void set_deadline_ms(long long deadline_ms) { deadline_ms_ = deadline_ms; }

    // Submit a turn. Enforces the input cap *before* calling the transport
    // (fail-closed, saves a network round-trip). Returns a validated response
    // or an error outcome. Returns a "cancelled" error if the token is aborted
    // before the first attempt or between attempts.
    ParseOutcome submit_turn(const TurnRequest& req);

    // Poll status. Returns the parsed status; caller decides when to stop.
    ParseOutcome poll_status(const std::string& turn_id);

    // Fetch reply audio. Enforces the download cap and the output cap against
    // both the advertised size and the actual returned size.
    struct AudioOutcome {
        bool ok = false;
        std::vector<std::uint8_t> wav_bytes;
        std::string error;
    };
    AudioOutcome fetch_audio(const std::string& turn_id, std::size_t advertised_bytes);

    // How many transport attempts the last submit_turn made (for tests/metrics).
    int last_attempts() const { return last_attempts_; }
    bool was_cancelled() const { return was_cancelled_; }
    bool timed_out() const { return timed_out_; }

private:
    bool stop_requested() const;      // abort OR deadline passed
    const char* stop_reason() const;  // "cancelled" | "deadline exceeded"

    HttpTransport& transport_;
    RetryPolicy policy_;
    CancellationToken* cancel_ = nullptr;
    Clock* clock_ = nullptr;
    long long deadline_ms_ = 0;
    int last_attempts_ = 0;
    bool was_cancelled_ = false;
    bool timed_out_ = false;
};

}  // namespace jalak
