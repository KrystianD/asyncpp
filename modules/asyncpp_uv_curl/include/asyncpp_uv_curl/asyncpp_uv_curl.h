#pragma once

#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv.h>

#include <string>

#include "curl_uv.h"

namespace asyncpp_uv_curl {
[[maybe_unused]] static asyncpp::task<curl_uv::CurlResponse> uvCurlExecute(
    curl_uv::CurlRequest&& req, asyncpp::cancellation_token token = asyncpp::cancellation_token::empty) {
  return asyncpp::makeCancellableTask<curl_uv::CurlResponse>(
      [&req](auto resolve) {
        auto ses = curl_uv::execute(std::move(req), resolve);

        return [ses]() mutable { ses.abort(); };
      },
      token);
}

[[maybe_unused]] static asyncpp::task<curl_uv::CurlResponse> uvCurlGET(const std::string& url) {
  curl_uv::CurlRequest req = curl_uv::get(url);
  co_return co_await uvCurlExecute(std::move(req));
}

[[maybe_unused]] static asyncpp::task<curl_uv::CurlResponse> uvCurlPOST(const std::string& url,
                                                                        const std::string& payload) {
  curl_uv::CurlRequest req = curl_uv::post(url);
  req.setPayload(payload);
  co_return co_await uvCurlExecute(std::move(req));
}

[[maybe_unused]] static asyncpp::task<curl_uv::CurlResponse> uvCurlDELETE(const std::string& url,
                                                                          const std::string& payload) {
  curl_uv::CurlRequest req = curl_uv::delete_(url);
  req.setPayload(payload);
  co_return co_await uvCurlExecute(std::move(req));
}
}  // namespace asyncpp_uv_curl