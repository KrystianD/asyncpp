#pragma once

#include <uv.h>

#include <cstdint>
#include <functional>
#include <memory>

namespace asyncpp_uv::timer {
using TimerCallback = std::function<void()>;

class TimerHandle {
  void* handle;
  TimerCallback callback;

  TimerHandle(const TimerHandle&) = delete;
  TimerHandle& operator=(const TimerHandle&) = delete;
  TimerHandle(TimerHandle&&) = delete;
  TimerHandle& operator=(TimerHandle&&) = delete;

 public:
  TimerHandle(void* handle, TimerCallback callback) : handle(handle), callback(callback) {}

  void cancel();

  friend std::shared_ptr<TimerHandle> uvTimerStart(uint64_t timeoutMs, const TimerCallback& cb, uv_loop_t* loop);
};

std::shared_ptr<TimerHandle> uvTimerStart(uint64_t timeoutMs, const TimerCallback& cb, uv_loop_t* loop = nullptr);
}  // namespace asyncpp_uv::timer
