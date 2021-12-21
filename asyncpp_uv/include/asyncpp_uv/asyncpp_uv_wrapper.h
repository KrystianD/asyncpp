#pragma once

#include <uv.h>

#include <cstdint>
#include <functional>

namespace asyncpp_uv::wrapper {
using TimerCallback = std::function<void()>;
void uvTimerStart(uv_loop_t* loop, uint64_t timeoutMs, const TimerCallback& cb);
}  // namespace asyncpp_uv::wrapper
