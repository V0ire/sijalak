// Cross-boundary integration test (SW-FW-05 ↔ backend gateway).
//
// The JSON bodies below were produced by the ACTUAL backend wire encoder
// (backend/jalak_backend/wire.py) and pasted verbatim. This test proves the
// firmware parser consumes exactly what the backend emits, so the two ends
// cannot diverge on field names or value shapes. No network is involved.
//
// If backend wire.py changes its output shape, this test must be updated with
// fresh backend-generated samples (see backend/tests/test_wire_format.py).

#include <cstdio>
#include <string>

#include "jalak/api_client.h"

namespace {
int g_failures = 0;
#define CHECK(cond)                                                     \
    do {                                                                \
        if (!(cond)) {                                                  \
            std::printf("FAIL: %s (line %d)\n", #cond, __LINE__);       \
            ++g_failures;                                               \
        }                                                               \
    } while (0)

// Backend-generated: TurnWire(status="done", action="continue").
const char* kTurnDone =
    "{\"schema_version\":\"1\",\"turn_id\":\"abc123\",\"status\":\"done\","
    "\"reply_text\":\"Mari kita baca bersama\",\"action\":\"continue\","
    "\"audio_bytes\":48044}";

// Backend-generated: TurnWire(status="done", action="handoff_book").
const char* kHandoff =
    "{\"schema_version\":\"1\",\"turn_id\":\"abc123\",\"status\":\"done\","
    "\"reply_text\":\"Ambil buku ini\",\"action\":\"handoff_book\","
    "\"book_id\":\"buku-001\",\"audio_bytes\":24044}";

// Backend-generated: status poll (processing).
const char* kStatusProcessing =
    "{\"schema_version\":\"1\",\"turn_id\":\"abc123\",\"status\":\"processing\"}";

// Backend-generated: error_payload().
const char* kError =
    "{\"error\":\"payload_too_large\",\"detail\":\"capture exceeds 8 s canonical cap\"}";

void test_backend_done_turn_parses() {
    auto o = jalak::parse_turn_response(kTurnDone, 200);
    CHECK(o.ok);
    CHECK(o.response.turn_id == "abc123");
    CHECK(o.response.status == jalak::TurnStatus::Done);
    CHECK(o.response.action == "continue");
    CHECK(o.response.audio_bytes == 48044u);
    CHECK(o.response.reply_text == "Mari kita baca bersama");
}

void test_backend_handoff_parses_with_allowlist() {
    auto o = jalak::parse_turn_response_with_allowlist(kHandoff, 200, {"buku-001"}, 35);
    CHECK(o.ok);
    CHECK(o.response.action == "handoff_book");
    CHECK(o.response.book_id.has_value() && *o.response.book_id == "buku-001");
}

void test_backend_handoff_rejected_without_allowlist() {
    // Same body, empty allowlist -> firmware must reject (parity with backend).
    auto o = jalak::parse_turn_response(kHandoff, 200);
    CHECK(!o.ok);
}

void test_backend_status_parses() {
    auto o = jalak::parse_status_response(kStatusProcessing, 200);
    CHECK(o.ok);
    CHECK(o.response.status == jalak::TurnStatus::Processing);
}

void test_backend_error_body_not_mistaken_for_turn() {
    // An error payload from the backend is a non-2xx, so the client must not
    // treat it as a turn regardless of its JSON content.
    auto o = jalak::parse_turn_response(kError, 413);
    CHECK(!o.ok);
}

}  // namespace

int main() {
    test_backend_done_turn_parses();
    test_backend_handoff_parses_with_allowlist();
    test_backend_handoff_rejected_without_allowlist();
    test_backend_status_parses();
    test_backend_error_body_not_mistaken_for_turn();
    if (g_failures == 0) {
        std::printf("ALL CROSS-BOUNDARY INTEGRATION TESTS PASSED\n");
        return 0;
    }
    std::printf("%d CROSS-BOUNDARY TEST(S) FAILED\n", g_failures);
    return 1;
}
