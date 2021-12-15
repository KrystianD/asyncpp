#pragma once

#include <uv.h>

#include <cstdint>
#include <functional>

namespace asyncpp_uv::wrapper {
void uvTimerStart(uv_loop_t* loop, uint64_t timeoutMs, const std::function<void()>& cb);
}
