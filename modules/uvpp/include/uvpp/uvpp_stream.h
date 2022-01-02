#pragma once

#include <uv.h>

#include <functional>
#include <stdexcept>

namespace uvpp {
// uv_shutdown
using ShutdownCb = std::function<void(int result)>;

void uvShutdown(uv_stream_t* stream, ShutdownCb cb);

// uv_write
using WriteCb = std::function<void(int result)>;

void uvWrite(uv_stream_t* stream, const void* data, size_t length, WriteCb cb);
}  // namespace uvpp
