#pragma once

#include <uv.h>

#include <functional>

namespace uvpp {
using AsyncCallback = std::function<void()>;

void uvAsyncSend(AsyncCallback cb, uv_loop_t* loop = nullptr);
}  // namespace uvpp
