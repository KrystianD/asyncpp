#pragma once

#include <uv.h>

#include <chrono>

#include <asyncpp/asyncpp.h>
#include <uvpp/uvpp.h>

namespace asyncpp_uv {
asyncpp::task<void> uvCloseAsync(uv_handle_t* handle);

asyncpp::task<void> uvSleepAsync(uint64_t timeoutMs);
asyncpp::task<void> uvSleepAsync(const std::chrono::milliseconds& duration);

asyncpp::task<int> uvShutdownAsync(uv_stream_t* stream);
asyncpp::task<int> uvWriteAsync(uv_stream_t* stream, const void* data, size_t length);

asyncpp::task<int> uvTcpConnectAsync(uv_tcp_t* tcp, std::string host, uint16_t port);

template<typename T>
asyncpp::task<T> uvRunInThreadAsync(uvpp::WorkCallback<T> workCb) {
  return asyncpp::makeTask<T>(
      [workCb](auto resolve) { uvpp::uvRunInThread<T>(workCb, [resolve](T value) { resolve(std::move(value)); }); });
}
}  // namespace asyncpp_uv
