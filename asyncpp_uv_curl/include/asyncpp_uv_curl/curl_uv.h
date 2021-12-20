#pragma once

#include <curl/curl.h>

#include <functional>
#include <string>
#include <utility>

namespace curl_uv {
struct CurlSession;

enum class Method {
  GET,
  POST,
};

class CurlRequest {
  Method method;
  std::string url;
  curl_slist* slist = nullptr;

  // POST
  std::string payload;

  friend void fill_request(CURL* curl, const CurlRequest& request);

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

  void setPayload(const std::string& data) { this->payload = data; }
  void setPayload(std::string&& data) { this->payload = std::move(data); }
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
  long httpCode;
  std::vector<uint8_t> buffer;
  std::string error;

  [[nodiscard]] std::string_view as_string() const { return std::string_view((char*)buffer.data(), buffer.size()); }
  [[nodiscard]] std::string to_string() const { return std::string(as_string()); }
};

typedef std::function<void(CurlResponse&& response)> CurlCompletedCb;

struct CurlSession {
  CURL* handle;
  char errorBuffer[CURL_ERROR_SIZE];

  CurlRequest request;
  CurlResponse response;

  CurlCompletedCb completedCb;
};

CurlRequest get(const std::string& url);
CurlRequest post(const std::string& url);

void execute(CurlRequest&& request, CurlCompletedCb completedCb);

void unload();
}  // namespace curl_uv