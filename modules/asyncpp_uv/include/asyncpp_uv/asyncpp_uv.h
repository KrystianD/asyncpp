#include <uv.h>

#include <chrono>

#include <asyncpp/asyncpp.h>

namespace asyncpp_uv {
asyncpp::task<void> uvSleepAsync(uint64_t timeoutMs);
asyncpp::task<void> uvSleepAsync(const std::chrono::milliseconds& duration);
}  // namespace asyncpp_uv
