#pragma once

#include <uv.h>

#include <functional>

#include "_uvpp_ring_buffer.h"

namespace uvpp::utils {
class uvExecutor {
  uv_async_t* async;

  RingBuffer<std::function<void()>> buffer;

  uvExecutor(size_t size, uv_loop_t* loop) : buffer(size) {
    async = new uv_async_t();
    async->data = this;
    uv_async_init(loop, async, [](uv_async_t* async_) { ((uvExecutor*)async_->data)->onAsyncEvent(); });
  }

  uvExecutor(uvExecutor&) noexcept = delete;
  uvExecutor& operator=(uvExecutor&) noexcept = delete;

  uvExecutor(uvExecutor&&) noexcept = delete;
  uvExecutor& operator=(uvExecutor&&) noexcept = delete;

  void onAsyncEvent() {
    while (true) {
      auto [res, cb] = buffer.tryDequeue();

      if (res)
        cb();
      else
        return;
    }
  }

 public:
  ~uvExecutor() {
    uv_close((uv_handle_t*)async, [](uv_handle_t* handle) { delete (uv_async_t*)handle; });
  }

  void execute(std::function<void()> cb) {
    buffer.enqueue(std::move(cb));
    uv_async_send(async);
  }

  void unref() { uv_unref((uv_handle_t*)async); }

  static std::shared_ptr<uvExecutor> create(size_t size, uv_loop_t* loop = nullptr) noexcept {
    uvExecutor* queue = new uvExecutor(size, loop ? loop : uv_default_loop());
    return std::shared_ptr<uvExecutor>(queue);
  }
};
}  // namespace uvpp::utils
