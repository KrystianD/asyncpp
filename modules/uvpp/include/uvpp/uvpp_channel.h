#pragma once

#include <uv.h>

#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>

namespace uvpp {
template<typename T>
class uvChannel {
  std::function<void(T)> cb;
  uv_async_t* async;

  std::mutex mutex;
  std::condition_variable cond;

  std::vector<T> buffer;
  size_t readPos = 0, writePos = 0;
  size_t count = 0;

  uvChannel(uv_loop_t* loop) {
    async = new uv_async_t();
    async->data = this;
    uv_async_init(loop, async, [](uv_async_t* async_) { ((uvChannel*)async_->data)->onAsyncEvent(); });
  }

  uvChannel(uvChannel&) noexcept = delete;
  uvChannel& operator=(uvChannel&) noexcept = delete;

  uvChannel(uvChannel&&) noexcept = default;
  uvChannel& operator=(uvChannel&&) noexcept = delete;

  void onAsyncEvent() {
    std::unique_lock<std::mutex> lock(mutex);
    while (count > 0) {
      cb(std::move(buffer[readPos]));
      readPos++;
      if (readPos == buffer.size()) readPos = 0;
      count--;
    }
    cond.notify_all();
  }

 public:
  ~uvChannel() {
    uv_close((uv_handle_t*)async, [](uv_handle_t* handle) { delete (uv_async_t*)handle; });
  }

  void send(T value) {
    std::unique_lock<std::mutex> lock(mutex);
    cond.wait(lock, [this]() { return count < buffer.size(); });
    buffer[writePos] = std::move(value);
    writePos++;
    if (writePos == buffer.size()) writePos = 0;
    count++;
    uv_async_send(async);
  }

  static std::shared_ptr<uvChannel> create(size_t size, std::function<void(T)> cb, uv_loop_t* loop = nullptr) {
    uvChannel* channel = new uvChannel(loop ? loop : uv_default_loop());
    channel->buffer.resize(size);
    channel->cb = std::move(cb);
    return std::shared_ptr<uvChannel>(channel);
  }
};
}  // namespace uvpp
