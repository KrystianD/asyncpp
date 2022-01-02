#pragma once

#include <uv.h>

#include <functional>

namespace uvpp {
using CloseCb = std::function<void(uv_handle_t*)>;

void uvClose(uv_handle_t* handle, CloseCb cb);
}  // namespace uvpp
