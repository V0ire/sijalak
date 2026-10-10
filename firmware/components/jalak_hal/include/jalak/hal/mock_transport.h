#pragma once
// Mock HTTP transport for host tests of DeviceApiClient.
//
// It replays queued responses and records the requests it received. No socket,
// no TLS, no DNS. The real transport (esp_http_client) is UNVERIFIED and lives
// outside this component. This mock is what makes SW-FW-05 host-testable.

#include <cstddef>
#include <deque>
#include <string>
#include <vector>

#include "jalak/api_client.h"

namespace jalak {
namespace mock {

class MockTransport : public HttpTransport {
public:
    // Scripted responses, consumed in order. When a queue empties, the last
    // queued response is returned again (so polling loops terminate only on the
    // scripted content, not on an empty queue error).
    std::deque<HttpResult> post_script;
    std::deque<HttpResult> status_script;
    std::deque<HttpResult> audio_script;

    // Recorded calls, so tests can assert what the client actually sent.
    std::vector<TurnRequest> posted;
    std::vector<std::string> polled_ids;
    std::vector<std::string> audio_ids;

    HttpResult post_turn(const TurnRequest& req) override {
        posted.push_back(req);
        return next(post_script);
    }
    HttpResult get_turn(const std::string& turn_id) override {
        polled_ids.push_back(turn_id);
        return next(status_script);
    }
    HttpResult get_audio(const std::string& turn_id) override {
        audio_ids.push_back(turn_id);
        return next(audio_script);
    }

    // Convenience builders.
    static HttpResult ok_body(std::string body, int status = 200) {
        HttpResult r;
        r.ok = true;
        r.status = status;
        r.body = std::move(body);
        return r;
    }
    static HttpResult transport_fail(std::string error) {
        HttpResult r;
        r.ok = false;
        r.status = 0;
        r.error = std::move(error);
        return r;
    }
    static HttpResult timed_out(std::string error = "connect/read timeout") {
        HttpResult r;
        r.ok = false;
        r.status = 0;
        r.error = std::move(error);
        r.timed_out = true;
        return r;
    }
    static HttpResult aborted(std::string error = "aborted by cancel") {
        HttpResult r;
        r.ok = false;
        r.status = 0;
        r.error = std::move(error);
        r.aborted = true;
        return r;
    }

private:
    static HttpResult next(std::deque<HttpResult>& q) {
        if (q.empty()) return transport_fail("mock transport: empty script");
        if (q.size() == 1) return q.front();  // repeat last for trailing polls
        HttpResult r = q.front();
        q.pop_front();
        return r;
    }
};

}  // namespace mock
}  // namespace jalak
