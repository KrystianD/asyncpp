#include <asyncpp_uv/asyncpp_uv.h>
#include <uvpp/uvpp.h>

namespace asyncpp_uv {
asyncpp::task<void> uvCloseAsync(uv_handle_t* handle) {
  return asyncpp::makeTask([handle](auto resolve) { uvpp::uvClose(handle, [resolve](uv_handle_t*) { resolve(); }); });
}

asyncpp::cancellable_task<void> uvSleepAsync(uint64_t timeoutMs, asyncpp::cancellation_token token) {
  return asyncpp::makeCancellableTask(
      [timeoutMs](auto resolve) {
        auto timer = uvpp::uvTimerStart(timeoutMs, resolve);

        return [timer]() { timer->cancel(); };
      },
      token);
}

asyncpp::cancellable_task<void> uvSleepAsync(const std::chrono::milliseconds& duration,
                                             asyncpp::cancellation_token token) {
  return uvSleepAsync((uint64_t)duration.count(), token);
}

asyncpp::task<int> uvShutdownAsync(uv_stream_t* stream) {
  return asyncpp::makeTask<int>([stream](auto resolve) { uvpp::uvShutdown(stream, resolve); });
}

asyncpp::task<int> uvWriteAsync(uv_stream_t* stream, const void* data, size_t length) {
  return asyncpp::makeTask<int>([stream, data, length](auto resolve) { uvpp::uvWrite(stream, data, length, resolve); });
}

asyncpp::task<int> uvTcpConnectAsync(uv_tcp_t* tcp, std::string host, uint16_t port) {
  return asyncpp::makeTask<int>([tcp, host, port](auto resolve) { uvpp::uvTcpConnect(tcp, host, port, resolve); });
}

}  // namespace asyncpp_uv