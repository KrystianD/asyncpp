#pragma once

#include <curl/curl.h>

#include <functional>
#include <string>
#include <utility>
#include <chrono>
#include <memory>

namespace curl_uv {

enum class Method {
  GET,
  POST,
  DELETE,
};

class CurlRequest {
  Method method;
  std::string url;
  std::chrono::milliseconds connectTimeout = std::chrono::seconds(300);
  std::chrono::milliseconds timeout = std::chrono::seconds(60);
  curl_slist* slist = nullptr;
  bool verbose = false;

  // POST
  std::string payload;

  friend void fillRequest(CURL* curl, const CurlRequest& request);

 public:
  CurlRequest() = default;
  CurlRequest(Method method, std::string url) : method(method), url(std::move(url)) {}

  // not copyable
  CurlRequest(const CurlRequest&) = delete;
  CurlRequest& operator=(const CurlRequest&) = delete;
  // movable
  CurlRequest(CurlRequest&&) = default;
  CurlRequest& operator=(CurlRequest&&) = default;

  void addHeader(const std::string_view& name, const std::string_view& value);

  void setConnectTimeout(uint32_t timeout_ms) { this->connectTimeout = std::chrono::milliseconds(timeout_ms); }
  void setConnectTimeout(std::chrono::milliseconds timeout) { this->connectTimeout = timeout; }

  void setTimeout(uint32_t timeout_ms) { this->timeout = std::chrono::milliseconds(timeout_ms); }
  void setTimeout(std::chrono::milliseconds timeout) { this->timeout = timeout; }

  void setPayload(const std::string& data) { this->payload = data; }
  void setPayload(std::string&& data) { this->payload = std::move(data); }

  void setVerbose(bool verbose) { this->verbose = verbose; }
};

class CurlResponse {
 public:
  CurlResponse() = default;

  // not copyable
  CurlResponse(const CurlResponse&) = delete;
  CurlResponse& operator=(const CurlResponse&) = delete;
  // movable
  CurlResponse(CurlResponse&&) = default;
  CurlResponse& operator=(CurlResponse&&) = default;

  CURLcode result;
  long statusCode;
  std::vector<uint8_t> buffer;
  std::string error;

  [[nodiscard]] std::string_view asString() const { return std::string_view((char*)buffer.data(), buffer.size()); }
  [[nodiscard]] std::string toString() const { return std::string(asString()); }
};

typedef std::function<void(CurlResponse&& response)> CurlCompletedCb;

CurlRequest get(const std::string& url);
CurlRequest post(const std::string& url);
CurlRequest delete_(const std::string& url);

void execute(CurlRequest&& request, CurlCompletedCb completedCb);
CurlResponse executeSync(CurlRequest&& request);

void unload();
}  // namespace curl_uv