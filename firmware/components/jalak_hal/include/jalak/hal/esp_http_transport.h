#pragma once
// IDF-backed HTTP transport implementing jalak::HttpTransport over esp_http_client.
//
// UNVERIFIED: requires Wi-Fi/TLS + a real endpoint. Compiles only under ESP-IDF.
// It honors the client-side contract: reports timed_out and aborted so
// DeviceApiClient can classify outcomes. A true in-flight abort additionally
// requires cancelling the underlying esp_http_client handle; this stub exposes
// the fields but the real cancellation wiring is UNVERIFIED.

#include <atomic>
#include <string>

#include "jalak/api_client.h"

#if defined(ESP_PLATFORM)

namespace jalak::hal {

struct HttpTransportConfig {
    std::string base_url;         // e.g. "https://HOST/v1" (HOST is UNKNOWN)
    std::string device_token;     // x-device-token
    std::string device_id;        // x-device-id
    int timeout_ms = 15000;       // per-request
    std::string ca_cert_pem;      // optional; empty = use bundle when available
};

class EspHttpTransport final : public jalak::HttpTransport {
public:
    explicit EspHttpTransport(HttpTransportConfig cfg) : cfg_(std::move(cfg)) {}

    jalak::HttpResult post_turn(const jalak::TurnRequest& req) override;
    jalak::HttpResult get_turn(const std::string& turn_id) override;
    jalak::HttpResult get_audio(const std::string& turn_id) override;

    // POST /v1/session -> returns the new session_id on success (200 + JSON).
    // Not part of jalak::HttpTransport (that interface is turn-shaped); this is
    // the device's session bootstrap. Returns "" on any failure.
    std::string create_session();

    // Cooperative cancel: flips the flag checked between internal operations.
    void request_abort() { abort_.store(true, std::memory_order_relaxed); }
    void reset_abort() { abort_.store(false, std::memory_order_relaxed); }

private:
    HttpTransportConfig cfg_;
    std::atomic<bool> abort_{false};
};

}  // namespace jalak::hal
#endif  // ESP_PLATFORM
