#include "jalak/api_client.h"

#include <algorithm>
#include <cctype>
#include <cstddef>

#include "jalak/audio.h"

namespace jalak {

namespace {

// Canonical download cap (768 KiB). Kept here as the client-side hard ceiling;
// must match backend limits.DOWNLOAD_CAP_BYTES.
constexpr std::size_t kDownloadCapBytes = 768u * 1024u;

std::size_t download_cap_bytes_hard() { return kDownloadCapBytes; }

// --- Tiny tolerant scanners (kept local; no external JSON dependency). ---

bool is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Append a Unicode code point as UTF-8.
void append_utf8(std::string& out, unsigned int cp) {
    if (cp <= 0x7F) {
        out.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
}

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// Decode a JSON string literal starting at `pos` (which points just past the
// opening quote). On success sets *end to the index just past the closing quote.
// Returns false on malformed input (bad escape, bad \u, unterminated).
bool decode_json_string(const std::string& json, std::size_t pos, std::string* out,
                        std::size_t* end) {
    out->clear();
    while (pos < json.size()) {
        const char c = json[pos];
        if (c == '"') {
            *end = pos + 1;
            return true;
        }
        if (c != '\\') {
            out->push_back(c);
            ++pos;
            continue;
        }
        if (pos + 1 >= json.size()) return false;  // trailing backslash
        const char n = json[pos + 1];
        switch (n) {
            case '"': out->push_back('"'); pos += 2; break;
            case '\\': out->push_back('\\'); pos += 2; break;
            case '/': out->push_back('/'); pos += 2; break;
            case 'b': out->push_back('\b'); pos += 2; break;
            case 'f': out->push_back('\f'); pos += 2; break;
            case 'n': out->push_back('\n'); pos += 2; break;
            case 'r': out->push_back('\r'); pos += 2; break;
            case 't': out->push_back('\t'); pos += 2; break;
            case 'u': {
                if (pos + 6 > json.size()) return false;
                unsigned int cp = 0;
                for (int i = 2; i < 6; ++i) {
                    const int h = hex_val(json[pos + i]);
                    if (h < 0) return false;
                    cp = (cp << 4) | static_cast<unsigned int>(h);
                }
                pos += 6;
                // Surrogate pair: high surrogate followed by \uXXXX low.
                if (cp >= 0xD800 && cp <= 0xDBFF && pos + 6 <= json.size() &&
                    json[pos] == '\\' && json[pos + 1] == 'u') {
                    unsigned int lo = 0;
                    bool ok = true;
                    for (int i = 2; i < 6; ++i) {
                        const int h = hex_val(json[pos + i]);
                        if (h < 0) { ok = false; break; }
                        lo = (lo << 4) | static_cast<unsigned int>(h);
                    }
                    if (ok && lo >= 0xDC00 && lo <= 0xDFFF) {
                        cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                        pos += 6;
                    }
                }
                append_utf8(*out, cp);
                break;
            }
            default:
                return false;  // invalid escape -> fail closed
        }
    }
    return false;  // unterminated string
}

// Find `"key"` (top-level, last occurrence wins per JSON semantics) followed by
// optional whitespace, ':', optional whitespace, then a JSON string literal.
// Returns the decoded string, or nullopt if absent or malformed.
std::optional<std::string> scan_string(const std::string& json, const std::string& key) {
    const std::string needle = "\"" + key + "\"";
    std::size_t search = 0;
    std::optional<std::string> result;
    while (true) {
        const std::size_t pos = json.find(needle, search);
        if (pos == std::string::npos) break;
        std::size_t colon = json.find(':', pos + needle.size());
        if (colon == std::string::npos) break;
        std::size_t v = colon + 1;
        while (v < json.size() && is_space(json[v])) ++v;
        search = pos + needle.size();
        if (v >= json.size() || json[v] != '"') continue;  // not a string here
        std::string decoded;
        std::size_t end = 0;
        if (!decode_json_string(json, v + 1, &decoded, &end)) {
            return std::nullopt;  // malformed value -> fail closed
        }
        result = decoded;  // last-wins
        search = end;
    }
    return result;
}

// Extract a top-level integer for `key`. Returns nullopt if absent. If the key
// is present but not a valid integer (non-numeric, or out of int64 range), the
// returned optional is nullopt AND *malformed is set to true, so callers can
// fail closed instead of silently skipping a size check.
std::optional<long long> scan_int(const std::string& json, const std::string& key,
                                  bool* malformed = nullptr) {
    if (malformed) *malformed = false;
    const std::string needle = "\"" + key + "\"";
    std::size_t pos = json.find(needle);
    if (pos == std::string::npos) return std::nullopt;
    pos = json.find(':', pos + needle.size());
    if (pos == std::string::npos) {
        if (malformed) *malformed = true;  // key present, no value
        return std::nullopt;
    }
    ++pos;
    while (pos < json.size() && is_space(json[pos])) ++pos;
    std::size_t start = pos;
    if (pos < json.size() && (json[pos] == '-' || json[pos] == '+')) ++pos;
    const std::size_t digits_begin = pos;
    while (pos < json.size() && std::isdigit(static_cast<unsigned char>(json[pos]))) ++pos;
    if (pos == digits_begin) {
        if (malformed) *malformed = true;  // no digits after optional sign
        return std::nullopt;
    }
    // Trailing characters (e.g. "12abc") make the value malformed.
    if (pos < json.size() && !is_space(json[pos]) && json[pos] != ',' &&
        json[pos] != '}' && json[pos] != ']') {
        if (malformed) *malformed = true;
        return std::nullopt;
    }
    const std::string tok = json.substr(start, pos - start);
    try {
        std::size_t consumed = 0;
        long long v = std::stoll(tok, &consumed);
        if (consumed != tok.size()) {
            if (malformed) *malformed = true;  // partial parse (overflow-ish)
            return std::nullopt;
        }
        return v;
    } catch (...) {
        // std::stoll throws on out-of-range (e.g. 1e19) or no conversion.
        if (malformed) *malformed = true;
        return std::nullopt;
    }
}

int count_words(const std::string& text) {
    int n = 0;
    bool in_word = false;
    for (char c : text) {
        if (is_space(c)) {
            in_word = false;
        } else if (!in_word) {
            in_word = true;
            ++n;
        }
    }
    return n;
}

bool action_allowed(const std::string& action) {
    return action == "continue" || action == "handoff_book" ||
           action == "safe_refusal" || action == "end_session";
}

TurnStatus status_from_string(const std::string& s) {
    if (s == "accepted") return TurnStatus::Accepted;
    if (s == "processing") return TurnStatus::Processing;
    if (s == "done") return TurnStatus::Done;
    if (s == "failed") return TurnStatus::Failed;
    if (s == "cancelled") return TurnStatus::Cancelled;
    return TurnStatus::Unknown;
}

}  // namespace

const char* to_string(TurnStatus s) {
    switch (s) {
        case TurnStatus::Accepted: return "accepted";
        case TurnStatus::Processing: return "processing";
        case TurnStatus::Done: return "done";
        case TurnStatus::Failed: return "failed";
        case TurnStatus::Cancelled: return "cancelled";
        case TurnStatus::Unknown: return "unknown";
    }
    return "unknown";
}

std::optional<std::string> json_get_string(const std::string& json, const std::string& key) {
    return scan_string(json, key);
}

// ---------------------------------------------------------------------------

bool RetryPolicy::retry_on_status(int http_status) const {
    if (http_status >= 500 && http_status <= 599) return true;   // transient server
    if (http_status == 429) return true;                         // rate limited
    return false;                                                // 2xx/4xx: no retry
}

int RetryPolicy::backoff_ms(int attempt) const {
    if (attempt <= 1) return 0;
    int ms = base_backoff_ms;
    for (int i = 2; i < attempt; ++i) {
        ms *= 2;
        if (ms >= max_backoff_ms) return max_backoff_ms;
    }
    return std::min(ms, max_backoff_ms);
}

// ---------------------------------------------------------------------------

namespace {
bool in_allowlist(const std::vector<std::string>& allow, const std::string& id) {
    return std::find(allow.begin(), allow.end(), id) != allow.end();
}
}  // namespace

ParseOutcome parse_turn_response(const std::string& body, int http_status,
                                 int max_reply_words) {
    // Empty allowlist: any book_id (or a bare handoff) is rejected, matching the
    // backend default. Callers that expect book handoffs MUST use the
    // allowlist-aware overload.
    return parse_turn_response_with_allowlist(body, http_status, {}, max_reply_words);
}

ParseOutcome parse_turn_response_with_allowlist(
    const std::string& body, int http_status,
    const std::vector<std::string>& book_allowlist, int max_reply_words) {
    ParseOutcome out;

    // Only 2xx carries a usable turn. 4xx is a client error (do not retry).
    if (http_status < 200 || http_status >= 300) {
        out.ok = false;
        out.error.reason = "non-2xx turn response";
        out.response.http_status = http_status;
        return out;
    }

    auto turn_id = scan_string(body, "turn_id");
    if (!turn_id || turn_id->empty()) {
        out.error.reason = "missing turn_id";
        return out;
    }
    auto status_s = scan_string(body, "status");
    if (!status_s) {
        out.error.reason = "missing status";
        return out;
    }
    const TurnStatus status = status_from_string(*status_s);
    if (status == TurnStatus::Unknown) {
        out.error.reason = "unknown status value";
        return out;
    }

    TurnResponse r;
    r.http_status = http_status;
    r.turn_id = *turn_id;
    r.status = status;
    if (auto v = scan_string(body, "schema_version")) r.schema_version = *v;

    // A turn that is not done yet may legitimately omit reply/action.
    if (status == TurnStatus::Done) {
        auto reply = scan_string(body, "reply_text");
        if (!reply || reply->empty()) {
            out.error.reason = "done turn without reply_text";
            return out;
        }
        if (count_words(*reply) > max_reply_words) {
            out.error.reason = "reply exceeds word limit";
            return out;
        }
        auto action = scan_string(body, "action");
        if (!action || !action_allowed(*action)) {
            out.error.reason = "missing or disallowed action";
            return out;
        }
        r.reply_text = *reply;
        r.action = *action;

        // Book id, if present, must be curated. A handoff MUST carry a curated
        // id (mirrors backend reply_schema.validate_reply).
        if (auto bid = scan_string(body, "book_id")) {
            if (!in_allowlist(book_allowlist, *bid)) {
                out.error.reason = "book_id not in curated allowlist";
                return out;
            }
            r.book_id = *bid;
        } else if (*action == "handoff_book") {
            out.error.reason = "handoff_book without a curated book_id";
            return out;
        }

        // audio_bytes, if present, must be a valid integer in range. A present
        // but malformed/overflowing value fails closed rather than skipping the
        // size check (D1).
        bool ab_malformed = false;
        auto ab = scan_int(body, "audio_bytes", &ab_malformed);
        if (ab_malformed) {
            out.error.reason = "malformed audio_bytes";
            return out;
        }
        if (ab.has_value()) {
            if (*ab < 0) {
                out.error.reason = "negative audio_bytes";
                return out;
            }
            r.audio_bytes = static_cast<std::size_t>(*ab);
            if (r.audio_bytes > output_cap_bytes()) {
                out.error.reason = "advertised audio exceeds output cap";
                return out;
            }
        }
    }

    out.ok = true;
    out.response = r;
    return out;
}

ParseOutcome parse_status_response(const std::string& body, int http_status) {
    ParseOutcome out;
    if (http_status < 200 || http_status >= 300) {
        out.error.reason = "non-2xx status response";
        out.response.http_status = http_status;
        return out;
    }
    auto turn_id = scan_string(body, "turn_id");
    if (!turn_id || turn_id->empty()) {
        out.error.reason = "missing turn_id";
        return out;
    }
    auto status_s = scan_string(body, "status");
    if (!status_s) {
        out.error.reason = "missing status";
        return out;
    }
    const TurnStatus status = status_from_string(*status_s);
    if (status == TurnStatus::Unknown) {
        out.error.reason = "unknown status value";
        return out;
    }
    out.ok = true;
    out.response.http_status = http_status;
    out.response.turn_id = *turn_id;
    out.response.status = status;
    return out;
}

// ---------------------------------------------------------------------------

DeviceApiClient::DeviceApiClient(HttpTransport& transport, RetryPolicy policy)
    : transport_(transport), policy_(policy) {}

bool DeviceApiClient::stop_requested() const {
    if (cancel_ != nullptr && cancel_->aborted()) return true;
    if (clock_ != nullptr && deadline_ms_ != 0 && clock_->now_ms() >= deadline_ms_) return true;
    return false;
}

const char* DeviceApiClient::stop_reason() const {
    if (cancel_ != nullptr && cancel_->aborted()) return "cancelled";
    return "deadline exceeded";
}

ParseOutcome DeviceApiClient::submit_turn(const TurnRequest& req) {
    last_attempts_ = 0;
    was_cancelled_ = false;
    timed_out_ = false;

    // Fail-closed input validation BEFORE any network call.
    std::string reason;
    if (req.idempotency_key.empty()) {
        ParseOutcome o;
        o.error.reason = "empty idempotency_key";
        o.kind = ParseOutcome::Kind::Validation;
        return o;
    }
    if (!validate_capture_size(req.wav_bytes.size(), &reason)) {
        ParseOutcome o;
        o.error.reason = reason;
        o.kind = ParseOutcome::Kind::Validation;
        return o;
    }

    // Cancellation before a request prevents network execution entirely.
    if (stop_requested()) {
        const bool cancelled = (cancel_ != nullptr && cancel_->aborted());
        was_cancelled_ = cancelled;
        timed_out_ = !cancelled;
        ParseOutcome o;
        o.error.reason = stop_reason();
        o.kind = cancelled ? ParseOutcome::Kind::Cancelled : ParseOutcome::Kind::Timeout;
        return o;
    }

    HttpResult last;
    for (int attempt = 1; attempt <= policy_.max_attempts; ++attempt) {
        ++last_attempts_;
        last = transport_.post_turn(req);

        // Explicit abort, whether from the token or observed by the transport,
        // must NEVER trigger a retry.
        if (last.aborted || (cancel_ != nullptr && cancel_->aborted())) {
            was_cancelled_ = true;
            ParseOutcome o;
            o.error.reason = "cancelled";
            o.kind = ParseOutcome::Kind::Cancelled;
            return o;
        }

        // A timeout means the server MAY have executed the turn. Do not retry
        // blindly; surface an ambiguous outcome so the caller reconciles via
        // poll_status with the idempotency key.
        if (last.timed_out) {
            timed_out_ = true;
            ParseOutcome o;
            o.error.reason = "request timed out (outcome ambiguous)";
            o.kind = ParseOutcome::Kind::Timeout;
            return o;
        }

        if (last.ok && !policy_.retry_on_status(last.status)) break;
        if (!last.ok && !policy_.retry_on_transport_error(last.ok)) break;

        // Cancellation between attempts prevents further retries.
        if (stop_requested()) {
            const bool cancelled = (cancel_ != nullptr && cancel_->aborted());
            was_cancelled_ = cancelled;
            timed_out_ = !cancelled;
            ParseOutcome o;
            o.error.reason = stop_reason();
            o.kind = cancelled ? ParseOutcome::Kind::Cancelled : ParseOutcome::Kind::Timeout;
            return o;
        }
        // Retryable: fall through to the next attempt (if any).
    }

    if (!last.ok) {
        ParseOutcome o;
        o.error.reason = last.error.empty() ? "transport failure" : last.error;
        o.kind = ParseOutcome::Kind::Transport;
        return o;
    }
    ParseOutcome out = parse_turn_response(last.body, last.status);
    out.kind = out.ok ? ParseOutcome::Kind::Ok : ParseOutcome::Kind::Transport;
    return out;
}

ParseOutcome DeviceApiClient::poll_status(const std::string& turn_id) {
    if (stop_requested()) {
        const bool cancelled = (cancel_ != nullptr && cancel_->aborted());
        ParseOutcome o;
        o.error.reason = cancelled ? "cancelled" : "deadline exceeded";
        o.kind = cancelled ? ParseOutcome::Kind::Cancelled : ParseOutcome::Kind::Timeout;
        return o;
    }
    const HttpResult res = transport_.get_turn(turn_id);
    if (res.aborted) {
        ParseOutcome o;
        o.error.reason = "cancelled";
        o.kind = ParseOutcome::Kind::Cancelled;
        return o;
    }
    if (!res.ok) {
        ParseOutcome o;
        o.error.reason = res.error.empty() ? "transport failure" : res.error;
        o.kind = res.timed_out ? ParseOutcome::Kind::Timeout : ParseOutcome::Kind::Transport;
        return o;
    }
    ParseOutcome out = parse_status_response(res.body, res.status);
    out.kind = out.ok ? ParseOutcome::Kind::Ok : ParseOutcome::Kind::Transport;
    return out;
}

DeviceApiClient::AudioOutcome DeviceApiClient::fetch_audio(const std::string& turn_id,
                                                           std::size_t advertised_bytes) {
    AudioOutcome out;
    // Pre-download cap check: refuse to even fetch something too large.
    if (advertised_bytes > output_cap_bytes()) {
        out.error = "advertised audio exceeds output cap";
        return out;
    }
    if (advertised_bytes > download_cap_bytes_hard()) {
        out.error = "advertised audio exceeds download cap";
        return out;
    }
    if (stop_requested()) {
        const bool cancelled = (cancel_ != nullptr && cancel_->aborted());
        out.error = cancelled ? "cancelled" : "deadline exceeded";
        return out;
    }
    const HttpResult res = transport_.get_audio(turn_id);
    if (res.aborted) {
        out.error = "cancelled";
        return out;
    }
    if (!res.ok) {
        out.error = res.error.empty() ? "transport failure" : res.error;
        return out;
    }
    if (res.status < 200 || res.status >= 300) {
        out.error = "non-2xx audio response";
        return out;
    }
    // Enforce caps against the ACTUAL returned size too.
    if (res.body.size() > output_cap_bytes()) {
        out.error = "returned audio exceeds output cap";
        return out;
    }
    if (res.body.size() > download_cap_bytes_hard()) {
        out.error = "returned audio exceeds download cap";
        return out;
    }
    out.ok = true;
    out.wav_bytes.assign(res.body.begin(), res.body.end());
    return out;
}

}  // namespace jalak
