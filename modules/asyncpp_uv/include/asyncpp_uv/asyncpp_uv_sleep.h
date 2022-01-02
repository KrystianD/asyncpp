#pragma once

#include <asyncpp/asyncpp.h>

#include <chrono>
#include <cstdint>

namespace asyncpp_uv {
asyncpp::task<void> uvSleep(uint64_t timeoutMs);
asyncpp::task<void> uvSleep(const std::chrono::milliseconds& duration);
}  // namespace asyncpp_uv