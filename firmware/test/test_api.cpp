// Host tests for the device API client contract (SW-FW-05).
// Drives DeviceApiClient against MockTransport: no socket, no TLS, no network.
// This proves the *policy* (caps, retries, parsing, action allowlist); the real
// HTTP/TLS path remains UNVERIFIED.

#include <cstdio>
#include <string>
#include <vector>

#include "jalak/api_client.h"
#include "jalak/audio.h"
#include "jalak/hal/mock_transport.h"

namespace {
int g_failures = 0;
#define CHECK(cond)                                                     \
    do {                                                                \
        if (!(cond)) {                                                  \
            std::printf("FAIL: %s (line %d)\n", #cond, __LINE__);       \
            ++g_failures;                                               \
        }                                                               \
    } while (0)

jalak::TurnRequest make_req(std::size_t wav_len = 1000) {
    jalak::TurnRequest r;
    r.session_id = "sess-1";
    r.idempotency_key = "idem-1";
    r.wav_bytes.assign(wav_len, 0x11);
    return r;
}

// Deterministic logical clock for deadline tests. No wall time involved.
struct FakeClock : jalak::Clock {
    long long t = 0;
    long long now_ms() override { return t; }
};

// --- JSON scanner -----------------------------------------------------------

void test_json_scanner() {
    const std::string j =
        "{\"turn_id\":\"abc\",\"status\":\"done\",\"reply_text\":\"Halo dunia\","
        "\"esc\":\"a\\\"b\"}";
    CHECK(jalak::json_get_string(j, "turn_id").value() == "abc");
    CHECK(jalak::json_get_string(j, "status").value() == "done");
    CHECK(jalak::json_get_string(j, "esc").value() == "a\"b");
    CHECK(!jalak::json_get_string(j, "missing").has_value());
}

// --- Response parsing -------------------------------------------------------

void test_parse_done_turn() {
    const std::string body =
        "{\"turn_id\":\"t1\",\"status\":\"done\",\"reply_text\":\"Mari lanjut baca\","
        "\"action\":\"continue\",\"audio_bytes\":48044,\"schema_version\":\"1\"}";
    auto o = jalak::parse_turn_response(body, 200);
    CHECK(o.ok);
    CHECK(o.response.turn_id == "t1");
    CHECK(o.response.status == jalak::TurnStatus::Done);
    CHECK(o.response.action == "continue");
    CHECK(o.response.audio_bytes == 48044u);
}

void test_parse_rejects_bad_action() {
    const std::string body =
        "{\"turn_id\":\"t1\",\"status\":\"done\",\"reply_text\":\"x\","
        "\"action\":\"run_shell\"}";
    auto o = jalak::parse_turn_response(body, 200);
    CHECK(!o.ok);
}

void test_parse_rejects_overlong_reply() {
    std::string many;
    for (int i = 0; i < 40; ++i) many += "kata ";
    const std::string body =
        "{\"turn_id\":\"t1\",\"status\":\"done\",\"reply_text\":\"" + many +
        "\",\"action\":\"continue\"}";
    auto o = jalak::parse_turn_response(body, 200, 35);
    CHECK(!o.ok);
}

void test_parse_rejects_non_2xx() {
    auto o = jalak::parse_turn_response("{\"turn_id\":\"t1\",\"status\":\"done\"}", 500);
    CHECK(!o.ok);
}

void test_parse_rejects_advertised_oversize() {
    // Advertised audio above the output cap must be rejected at parse time.
    const std::string body =
        "{\"turn_id\":\"t1\",\"status\":\"done\",\"reply_text\":\"x\","
        "\"action\":\"continue\",\"audio_bytes\":9999999}";
    auto o = jalak::parse_turn_response(body, 200);
    CHECK(!o.ok);
}

void test_parse_accepted_without_reply_ok() {
    auto o = jalak::parse_status_response("{\"turn_id\":\"t1\",\"status\":\"processing\"}", 200);
    CHECK(o.ok);
    CHECK(o.response.status == jalak::TurnStatus::Processing);
}

// --- Retry policy -----------------------------------------------------------

void test_retry_policy() {
    jalak::RetryPolicy p;
    CHECK(p.retry_on_status(500));
    CHECK(p.retry_on_status(503));
    CHECK(p.retry_on_status(429));
    CHECK(!p.retry_on_status(200));
    CHECK(!p.retry_on_status(400));
    CHECK(!p.retry_on_status(404));
    CHECK(p.backoff_ms(1) == 0);
    CHECK(p.backoff_ms(2) == 250);
    CHECK(p.backoff_ms(3) == 500);
    CHECK(p.backoff_ms(10) == p.max_backoff_ms);
}

// --- Client behavior --------------------------------------------------------

void test_submit_rejects_oversize_before_network() {
    jalak::mock::MockTransport t;
    jalak::DeviceApiClient client(t);
    auto req = make_req(jalak::input_cap_bytes() + 1);
    auto o = client.submit_turn(req);
    CHECK(!o.ok);
    CHECK(t.posted.empty());          // fail-closed: no network attempt
    CHECK(client.last_attempts() == 0);
}

void test_submit_rejects_empty_idempotency() {
    jalak::mock::MockTransport t;
    jalak::DeviceApiClient client(t);
    auto req = make_req();
    req.idempotency_key.clear();
    auto o = client.submit_turn(req);
    CHECK(!o.ok);
    CHECK(t.posted.empty());
}

void test_submit_retries_then_succeeds() {
    jalak::mock::MockTransport t;
    t.post_script.push_back(jalak::mock::MockTransport::transport_fail("reset"));
    t.post_script.push_back(jalak::mock::MockTransport::ok_body("{}", 503));
    t.post_script.push_back(jalak::mock::MockTransport::ok_body(
        "{\"turn_id\":\"t9\",\"status\":\"done\",\"reply_text\":\"oke\","
        "\"action\":\"continue\"}",
        200));
    jalak::DeviceApiClient client(t);
    auto o = client.submit_turn(make_req());
    CHECK(o.ok);
    CHECK(o.response.turn_id == "t9");
    CHECK(client.last_attempts() == 3);
    CHECK(t.posted.size() == 3);
}

void test_submit_no_retry_on_4xx() {
    jalak::mock::MockTransport t;
    t.post_script.push_back(jalak::mock::MockTransport::ok_body("{\"e\":1}", 400));
    jalak::DeviceApiClient client(t);
    auto o = client.submit_turn(make_req());
    CHECK(!o.ok);
    CHECK(client.last_attempts() == 1);  // 4xx is not retried
    CHECK(t.posted.size() == 1);
}

void test_fetch_audio_caps() {
    // Advertised over the download cap -> refuse before fetching.
    {
        jalak::mock::MockTransport t;
        jalak::DeviceApiClient client(t);
        auto a = client.fetch_audio("t1", jalak::output_cap_bytes() + 1);
        CHECK(!a.ok);
        CHECK(t.audio_ids.empty());
    }
    // Real body over the output cap -> reject even if not advertised.
    {
        jalak::mock::MockTransport t;
        std::string big(jalak::output_cap_bytes() + 1, '\0');
        t.audio_script.push_back(jalak::mock::MockTransport::ok_body(big));
        jalak::DeviceApiClient client(t);
        auto a = client.fetch_audio("t1", 0);
        CHECK(!a.ok);
    }
    // Reasonable body -> accepted, byte-exact.
    {
        jalak::mock::MockTransport t;
        std::string wav(48044, '\x7f');
        t.audio_script.push_back(jalak::mock::MockTransport::ok_body(wav));
        jalak::DeviceApiClient client(t);
        auto a = client.fetch_audio("t1", 48044);
        CHECK(a.ok);
        CHECK(a.wav_bytes.size() == 48044u);
        CHECK(t.audio_ids.size() == 1);
    }
}

// --- D1 regression: overflow must NOT bypass the size cap -------------------

void test_d1_audio_bytes_overflow_fails_closed() {
    // A value > INT64_MAX makes std::stoll throw. Before the fix this silently
    // skipped the cap check and ACCEPTED the turn. Must be rejected.
    const std::string body =
        "{\"turn_id\":\"t\",\"status\":\"done\",\"reply_text\":\"x\","
        "\"action\":\"continue\",\"audio_bytes\":99999999999999999999}";
    auto o = jalak::parse_turn_response(body, 200);
    CHECK(!o.ok);
}

void test_d1_audio_bytes_non_numeric_fails_closed() {
    // A present-but-non-numeric audio_bytes must not be silently ignored.
    const std::string body =
        "{\"turn_id\":\"t\",\"status\":\"done\",\"reply_text\":\"x\","
        "\"action\":\"continue\",\"audio_bytes\":\"lots\"}";
    auto o = jalak::parse_turn_response(body, 200);
    CHECK(!o.ok);
}

void test_d1_audio_bytes_overflow_within_cap_still_ok() {
    // Sanity: a legitimate small value still parses.
    const std::string body =
        "{\"turn_id\":\"t\",\"status\":\"done\",\"reply_text\":\"x\","
        "\"action\":\"continue\",\"audio_bytes\":48044}";
    auto o = jalak::parse_turn_response(body, 200);
    CHECK(o.ok);
    CHECK(o.response.audio_bytes == 48044u);
}

// --- D2 regression: firmware must enforce a curated book allowlist -----------

void test_d2_handoff_book_requires_book_id() {
    // Backend rejects handoff_book without book_id; firmware must too.
    const std::string body =
        "{\"turn_id\":\"t\",\"status\":\"done\",\"reply_text\":\"ambil buku ini\","
        "\"action\":\"handoff_book\"}";
    auto o = jalak::parse_turn_response_with_allowlist(body, 200, {"buku-001"}, 35);
    CHECK(!o.ok);
}

void test_d2_handoff_book_rejects_uncurated_id() {
    const std::string body =
        "{\"turn_id\":\"t\",\"status\":\"done\",\"reply_text\":\"x\","
        "\"action\":\"handoff_book\",\"book_id\":\"koleksi-palsu\"}";
    auto o = jalak::parse_turn_response_with_allowlist(body, 200, {"buku-001"}, 35);
    CHECK(!o.ok);
}

void test_d2_handoff_book_accepts_curated_id() {
    const std::string body =
        "{\"turn_id\":\"t\",\"status\":\"done\",\"reply_text\":\"ambil buku\","
        "\"action\":\"handoff_book\",\"book_id\":\"buku-001\"}";
    auto o = jalak::parse_turn_response_with_allowlist(body, 200, {"buku-001"}, 35);
    CHECK(o.ok);
    CHECK(o.response.book_id.has_value() && *o.response.book_id == "buku-001");
}

void test_d2_non_handoff_book_id_must_also_be_curated() {
    // Backend rejects ANY book_id not in the allowlist, regardless of action.
    const std::string body =
        "{\"turn_id\":\"t\",\"status\":\"done\",\"reply_text\":\"x\","
        "\"action\":\"continue\",\"book_id\":\"koleksi-palsu\"}";
    auto o = jalak::parse_turn_response_with_allowlist(body, 200, {"buku-001"}, 35);
    CHECK(!o.ok);
}

// --- L2/L3 regression: JSON decoding robustness ------------------------------

void test_l2_unicode_escape_decoded() {
    // \u00e9 is 'e' with acute (UTF-8 0xC3 0xA9). Before the fix this became "u00e9".
    const std::string j = "{\"reply_text\":\"caf\\u00e9\"}";
    auto v = jalak::json_get_string(j, "reply_text");
    CHECK(v.has_value());
    CHECK(*v == "caf\xC3\xA9");
}

void test_l2_standard_escapes() {
    const std::string j =
        "{\"a\":\"x\\ty\",\"b\":\"p\\r\\nq\",\"c\":\"q\\\"r\",\"d\":\"s\\\\t\"}";
    CHECK(jalak::json_get_string(j, "a").value() == "x\ty");
    CHECK(jalak::json_get_string(j, "b").value() == "p\r\nq");
    CHECK(jalak::json_get_string(j, "c").value() == "q\"r");
    CHECK(jalak::json_get_string(j, "d").value() == "s\\t");
}

void test_l2_bad_escape_fails_closed() {
    // Invalid escape (e.g. \x) must reject the whole value, not pass it through.
    const std::string j = "{\"reply_text\":\"bad\\xescape\"}";
    auto v = jalak::json_get_string(j, "reply_text");
    CHECK(!v.has_value());
}

void test_l2_surrogate_pair_decoded() {
    // \uD83D\uDE00 is U+1F600 (grinning face), UTF-8 F0 9F 98 80.
    const std::string j = "{\"emoji\":\"\\uD83D\\uDE00\"}";
    auto v = jalak::json_get_string(j, "emoji");
    CHECK(v.has_value());
    CHECK(*v == "\xF0\x9F\x98\x80");
}

void test_l3_duplicate_key_last_wins() {
    // JSON semantics: the last duplicate key wins. Before the fix, first won,
    // so a hostile earlier value shadowed the real one.
    const std::string j = "{\"status\":\"done\",\"status\":\"failed\"}";
    CHECK(jalak::json_get_string(j, "status").value() == "failed");
}

void test_l3_duplicate_key_cannot_pin_done() {
    // A server that reports processing via the last key must not be read as done.
    const std::string body =
        "{\"turn_id\":\"t\",\"status\":\"done\",\"status\":\"processing\"}";
    auto o = jalak::parse_status_response(body, 200);
    CHECK(o.ok);
    CHECK(o.response.status == jalak::TurnStatus::Processing);
}

// --- Cancellation + deadline semantics --------------------------------------

void test_cancel_before_request_prevents_network() {
    jalak::mock::MockTransport t;
    jalak::CancellationToken token;
    token.request_abort();
    jalak::DeviceApiClient client(t);
    client.set_cancellation(&token);
    auto o = client.submit_turn(make_req());
    CHECK(!o.ok);
    CHECK(o.kind == jalak::ParseOutcome::Kind::Cancelled);
    CHECK(t.posted.empty());                 // no network attempt at all
    CHECK(client.last_attempts() == 0);
    CHECK(client.was_cancelled());
}

void test_cancel_between_attempts_stops_retries() {
    // First attempt is a retryable transport failure; STOP (abort) arrives
    // during that first call, so the between-attempt check stops the loop and
    // only ONE attempt is made.
    struct AbortAfterFirst : jalak::HttpTransport {
        jalak::CancellationToken* tok;
        int calls = 0;
        jalak::HttpResult post_turn(const jalak::TurnRequest&) override {
            ++calls;
            jalak::HttpResult res = jalak::mock::MockTransport::transport_fail("reset");
            if (calls == 1) tok->request_abort();  // STOP arrives mid-flight
            return res;
        }
        jalak::HttpResult get_turn(const std::string&) override { return {}; }
        jalak::HttpResult get_audio(const std::string&) override { return {}; }
    } at;
    jalak::CancellationToken token;
    at.tok = &token;
    jalak::DeviceApiClient c2(at);
    c2.set_cancellation(&token);
    auto o = c2.submit_turn(make_req());
    CHECK(!o.ok);
    CHECK(o.kind == jalak::ParseOutcome::Kind::Cancelled);
    CHECK(c2.last_attempts() == 1);   // stopped before a retry
}

void test_explicit_abort_never_retries() {
    // Transport reports it observed an abort: the loop must stop immediately
    // even though the status/failure would normally be retryable.
    jalak::mock::MockTransport t;
    t.post_script.push_back(jalak::mock::MockTransport::aborted());
    t.post_script.push_back(jalak::mock::MockTransport::ok_body(
        "{\"turn_id\":\"t9\",\"status\":\"done\",\"reply_text\":\"oke\","
        "\"action\":\"continue\"}"));
    jalak::DeviceApiClient client(t);
    auto o = client.submit_turn(make_req());
    CHECK(!o.ok);
    CHECK(o.kind == jalak::ParseOutcome::Kind::Cancelled);
    CHECK(client.last_attempts() == 1);
    CHECK(t.posted.size() == 1);      // the queued success was never consumed
    CHECK(client.was_cancelled());
}

void test_timeout_is_not_retried_and_marked_ambiguous() {
    // A timed-out POST may have executed server-side; must NOT be retried.
    jalak::mock::MockTransport t;
    t.post_script.push_back(jalak::mock::MockTransport::timed_out());
    t.post_script.push_back(jalak::mock::MockTransport::ok_body(
        "{\"turn_id\":\"t9\",\"status\":\"done\",\"reply_text\":\"oke\","
        "\"action\":\"continue\"}"));
    jalak::DeviceApiClient client(t);
    auto o = client.submit_turn(make_req());
    CHECK(!o.ok);
    CHECK(o.kind == jalak::ParseOutcome::Kind::Timeout);
    CHECK(o.ambiguous());
    CHECK(client.last_attempts() == 1);   // exactly one POST
    CHECK(t.posted.size() == 1);
    CHECK(client.timed_out());
}

void test_deadline_before_request_prevents_network() {
    jalak::mock::MockTransport t;
    FakeClock clk;
    clk.t = 1000;
    jalak::DeviceApiClient client(t);
    client.set_clock(&clk);
    client.set_deadline_ms(1000);   // boundary: now == deadline -> already past
    auto o = client.submit_turn(make_req());
    CHECK(!o.ok);
    CHECK(o.kind == jalak::ParseOutcome::Kind::Timeout);
    CHECK(t.posted.empty());
    CHECK(client.last_attempts() == 0);
}

void test_deadline_boundary_not_yet_reached_allows_request() {
    jalak::mock::MockTransport t;
    t.post_script.push_back(jalak::mock::MockTransport::ok_body(
        "{\"turn_id\":\"t9\",\"status\":\"done\",\"reply_text\":\"oke\","
        "\"action\":\"continue\"}"));
    FakeClock clk;
    clk.t = 999;
    jalak::DeviceApiClient client(t);
    client.set_clock(&clk);
    client.set_deadline_ms(1000);   // now(999) < deadline(1000) -> allowed
    auto o = client.submit_turn(make_req());
    CHECK(o.ok);
    CHECK(t.posted.size() == 1);
}

void test_deadline_expired_between_attempts_stops_retries() {
    // Advance the clock past the deadline inside the first transport call; the
    // between-attempt check must stop the retry loop.
    struct ClockAdvancingTransport : jalak::HttpTransport {
        FakeClock* clk;
        int calls = 0;
        jalak::HttpResult post_turn(const jalak::TurnRequest&) override {
            ++calls;
            clk->t = 5000;  // past the deadline
            return jalak::mock::MockTransport::transport_fail("reset");
        }
        jalak::HttpResult get_turn(const std::string&) override { return {}; }
        jalak::HttpResult get_audio(const std::string&) override { return {}; }
    } at;
    FakeClock clk;
    clk.t = 0;
    at.clk = &clk;
    jalak::DeviceApiClient client(at);
    client.set_clock(&clk);
    client.set_deadline_ms(1000);
    auto o = client.submit_turn(make_req());
    CHECK(!o.ok);
    CHECK(o.kind == jalak::ParseOutcome::Kind::Timeout);
    CHECK(client.last_attempts() == 1);
}

void test_poll_and_audio_respect_cancellation() {
    jalak::mock::MockTransport t;
    jalak::CancellationToken token;
    token.request_abort();
    jalak::DeviceApiClient client(t);
    client.set_cancellation(&token);
    auto s = client.poll_status("t1");
    CHECK(!s.ok && s.kind == jalak::ParseOutcome::Kind::Cancelled);
    CHECK(t.polled_ids.empty());
    auto a = client.fetch_audio("t1", 100);
    CHECK(!a.ok);
    CHECK(t.audio_ids.empty());
}

void test_poll_status_records_id() {
    jalak::mock::MockTransport t;
    t.status_script.push_back(
        jalak::mock::MockTransport::ok_body("{\"turn_id\":\"t5\",\"status\":\"done\"}"));
    jalak::DeviceApiClient client(t);
    auto o = client.poll_status("t5");
    CHECK(o.ok);
    CHECK(o.response.status == jalak::TurnStatus::Done);
    CHECK(t.polled_ids.size() == 1 && t.polled_ids[0] == "t5");
}

}  // namespace

int main() {
    test_json_scanner();
    test_parse_done_turn();
    test_parse_rejects_bad_action();
    test_parse_rejects_overlong_reply();
    test_parse_rejects_non_2xx();
    test_parse_rejects_advertised_oversize();
    test_parse_accepted_without_reply_ok();
    test_retry_policy();
    test_submit_rejects_oversize_before_network();
    test_submit_rejects_empty_idempotency();
    test_submit_retries_then_succeeds();
    test_submit_no_retry_on_4xx();
    test_fetch_audio_caps();
    test_d1_audio_bytes_overflow_fails_closed();
    test_d1_audio_bytes_non_numeric_fails_closed();
    test_d1_audio_bytes_overflow_within_cap_still_ok();
    test_d2_handoff_book_requires_book_id();
    test_d2_handoff_book_rejects_uncurated_id();
    test_d2_handoff_book_accepts_curated_id();
    test_d2_non_handoff_book_id_must_also_be_curated();
    test_l2_unicode_escape_decoded();
    test_l2_standard_escapes();
    test_l2_bad_escape_fails_closed();
    test_l2_surrogate_pair_decoded();
    test_l3_duplicate_key_last_wins();
    test_l3_duplicate_key_cannot_pin_done();
    test_cancel_before_request_prevents_network();
    test_cancel_between_attempts_stops_retries();
    test_explicit_abort_never_retries();
    test_timeout_is_not_retried_and_marked_ambiguous();
    test_deadline_before_request_prevents_network();
    test_deadline_boundary_not_yet_reached_allows_request();
    test_deadline_expired_between_attempts_stops_retries();
    test_poll_and_audio_respect_cancellation();
    test_poll_status_records_id();
    if (g_failures == 0) {
        std::printf("ALL API CLIENT HOST TESTS PASSED\n");
        return 0;
    }
    std::printf("%d API CLIENT HOST TEST(S) FAILED\n", g_failures);
    return 1;
}
