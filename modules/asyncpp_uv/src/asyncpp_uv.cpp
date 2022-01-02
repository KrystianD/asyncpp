#include <asyncpp_uv/asyncpp_uv.h>
#include <uvpp/uvpp.h>

namespace asyncpp_uv {
asyncpp::task<void> uvSleepAsync(uint64_t timeoutMs) {
  return asyncpp::makeTask([timeoutMs](auto resolve) { uvpp::uvTimerStart(timeoutMs, resolve); });
}

asyncpp::task<void> uvSleepAsync(const std::chrono::milliseconds& duration) {
  return uvSleepAsync((uint64_t)duration.count());
}
}  // namespace asyncpp_uv