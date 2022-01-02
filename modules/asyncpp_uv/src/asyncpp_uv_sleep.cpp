#include <asyncpp_uv/asyncpp_uv_sleep.h>

#include <uvpp/uvpp_timer.h>

namespace asyncpp_uv {
asyncpp::task<void> uvSleep(uint64_t timeoutMs) {
  return asyncpp::makeTask([timeoutMs](auto resolve) { uvpp::uvTimerStart(timeoutMs, resolve); });
}

asyncpp::task<void> uvSleep(const std::chrono::milliseconds& duration) { return uvSleep((uint64_t)duration.count()); }
}  // namespace asyncpp_uv