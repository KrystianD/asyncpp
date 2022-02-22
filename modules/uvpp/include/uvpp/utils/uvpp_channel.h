#pragma once

#include <uv.h>

#include "_uvpp_ring_buffer.h"

namespace uvpp::utils {
template<typename T>
class uvChannel {
  std::function<void(T)> cb;
  uv_async_t* async;

  RingBuffer<T> buffer;

  uvChannel(size_t size, uv_loop_t* loop) : buffer(size) {
    async = new uv_async_t();
    async->data = this;
    uv_async_init(loop, async, [](uv_async_t* async_) { ((uvChannel*)async_->data)->onAsyncEvent(); });
  }

  uvChannel(uvChannel&) noexcept = delete;
  uvChannel& operator=(uvChannel&) noexcept = delete;

  uvChannel(uvChannel&&) noexcept = default;
  uvChannel& operator=(uvChannel&&) noexcept = delete;

  void onAsyncEvent() {
    while (true) {
      auto [res, value] = std::move(buffer.tryDequeue());

      if (res)
        cb(std::move(value));
      else
        return;
    }
  }

 public:
  ~uvChannel() {
    uv_close((uv_handle_t*)async, [](uv_handle_t* handle) { delete (uv_async_t*)handle; });
  }

  void send(T value) {
    buffer.enqueue(std::move(value));
    uv_async_send(async);
  }

  static std::shared_ptr<uvChannel> create(size_t size, std::function<void(T)> cb, uv_loop_t* loop = nullptr) {
    uvChannel* channel = new uvChannel(size, loop ? loop : uv_default_loop());
    channel->cb = std::move(cb);
    return std::shared_ptr<uvChannel>(channel);
  }
};
}  // namespace uvpp::utils
