#pragma once

#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv.h>

#include <string>

#include "curl_uv.h"

namespace asyncpp_uv_curl {
[[maybe_unused]] static asyncpp::task<curl_uv::CurlResponse> uvCurlExecute(curl_uv::CurlRequest&& req) {
  return asyncpp::makeTask<curl_uv::CurlResponse>([&req](auto resolve) { curl_uv::execute(std::move(req), resolve); });
}

[[maybe_unused]] static asyncpp::task<curl_uv::CurlResponse> uvCurlGET(const std::string& url) {
  curl_uv::CurlRequest req = curl_uv::get(url);
  co_return co_await uvCurlExecute(std::move(req));
}
}  // namespace asyncpp_uv_curl