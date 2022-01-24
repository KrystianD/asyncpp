#pragma once

#include <uv.h>

#include <cstdint>
#include <functional>
#include <memory>

namespace uvpp {
using TimerCallback = std::function<void()>;

class TimerHandle {
  void* handle;
  uint64_t timeoutMs;
  TimerCallback callback;

  TimerHandle(const TimerHandle&) = delete;
  TimerHandle& operator=(const TimerHandle&) = delete;
  TimerHandle(TimerHandle&&) = delete;
  TimerHandle& operator=(TimerHandle&&) = delete;

 public:
  TimerHandle(void* handle, uint64_t timeoutMs, TimerCallback callback)
      : handle(handle), timeoutMs(timeoutMs), callback(callback) {}

  void cancel();

  void restart();
  void restart(uint64_t newTimeoutMs);

  bool isRunning();

  friend void timerCb(uv_timer_t* uvHandle);
};

std::shared_ptr<TimerHandle> uvTimerStart(uint64_t timeoutMs, TimerCallback cb, uv_loop_t* loop = nullptr);
}  // namespace uvpp
