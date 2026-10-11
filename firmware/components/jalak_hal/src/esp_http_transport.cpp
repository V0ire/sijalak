// IDF esp_http_client transport implementation. Compiles only under ESP-IDF.
// UNVERIFIED: needs Wi-Fi + a real endpoint. Timeout/abort are surfaced to the
// client contract; true mid-request cancellation is not wired here.

#if defined(ESP_PLATFORM)

#include "jalak/hal/esp_http_transport.h"

#include <cstring>

#include "esp_http_client.h"
#include "esp_log.h"

namespace jalak::hal {

namespace {
constexpr char kTag[] = "jalak_http";

// Collect the response body into a string via the esp_http_client event handler.
esp_err_t http_event_cb(esp_http_client_event_t* evt) {
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        auto* out = static_cast<std::string*>(evt->user_data);
        if (out && evt->data && evt->data_len > 0) {
            out->append(static_cast<const char*>(evt->data), evt->data_len);
        }
    }
    return ESP_OK;
}

jalak::HttpResult perform(const HttpTransportConfig& cfg, esp_http_client_method_t method,
                          const std::string& url, const std::string& body,
                          const std::atomic<bool>& abort_flag) {
    jalak::HttpResult r;
    if (abort_flag.load(std::memory_order_relaxed)) {
        r.aborted = true;
        r.error = "aborted before request";
        return r;
    }
    std::string response;
    esp_http_client_config_t conf = {};
    conf.url = url.c_str();
    conf.method = method;
    conf.timeout_ms = cfg.timeout_ms;
    conf.event_handler = http_event_cb;
    conf.user_data = &response;
    conf.keep_alive_enable = false;

    esp_http_client_handle_t client = esp_http_client_init(&conf);
    if (client == nullptr) {
        r.error = "client init failed";
        return r;
    }
    if (!cfg.device_token.empty())
        esp_http_client_set_header(client, "x-device-token", cfg.device_token.c_str());
    if (!cfg.device_id.empty())
        esp_http_client_set_header(client, "x-device-id", cfg.device_id.c_str());
    if (!body.empty()) {
        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_http_client_set_post_field(client, body.c_str(), (int)body.size());
    }

    const esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_ERR_TIMEOUT) {
        r.timed_out = true;
        r.error = "connect/read timeout";
    } else if (err != ESP_OK) {
        r.error = esp_err_to_name(err);
    } else {
        r.ok = true;
        r.status = esp_http_client_get_status_code(client);
        r.body = std::move(response);
    }
    esp_http_client_cleanup(client);
    return r;
}

}  // namespace

jalak::HttpResult EspHttpTransport::post_turn(const jalak::TurnRequest& req) {
    // Build the exact body the backend accepts:
    //   {"session_id","idempotency_key","wav_base64"}
    // Reusing the portable builder keeps the wire shape host-tested.
    const std::string body =
        jalak::build_turn_request_body(req.session_id, req.idempotency_key, req.wav_bytes);
    const std::string url = cfg_.base_url + "/turn";
    return perform(cfg_, HTTP_METHOD_POST, url, body, abort_);
}

jalak::HttpResult EspHttpTransport::get_turn(const std::string& turn_id) {
    const std::string url = cfg_.base_url + "/turn/" + turn_id;
    return perform(cfg_, HTTP_METHOD_GET, url, {}, abort_);
}

jalak::HttpResult EspHttpTransport::get_audio(const std::string& turn_id) {
    const std::string url = cfg_.base_url + "/turn/" + turn_id + "/audio";
    return perform(cfg_, HTTP_METHOD_GET, url, {}, abort_);
}

std::string EspHttpTransport::create_session() {
    const std::string url = cfg_.base_url + "/session";
    // The backend accepts an empty body for POST /v1/session; identity comes
    // from the x-device-id header set in perform().
    const jalak::HttpResult res = perform(cfg_, HTTP_METHOD_POST, url, "{}", abort_);
    if (!res.ok || res.status < 200 || res.status >= 300) return "";
    auto sid = jalak::parse_session_id(res.body);
    return sid ? *sid : std::string();
}

}  // namespace jalak::hal

#endif  // ESP_PLATFORM
