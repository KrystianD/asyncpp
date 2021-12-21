#pragma once

#include <uv.h>

#include <cstdint>
#include <functional>

namespace asyncpp_uv::wrapper {
using TimerCallback = std::function<void()>;
void uvTimerStart(uint64_t timeoutMs, const TimerCallback& cb, uv_loop_t* loop = nullptr);
}  // namespace asyncpp_uv::wrapper
