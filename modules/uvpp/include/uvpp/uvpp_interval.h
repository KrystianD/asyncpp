#pragma once

#include <uv.h>

#include <cstdint>
#include <functional>
#include <memory>

namespace uvpp {
using IntervalCallback = std::function<void()>;

class IntervalHandle {
  void* handle;
  IntervalCallback callback;

  IntervalHandle(const IntervalHandle&) = delete;
  IntervalHandle& operator=(const IntervalHandle&) = delete;
  IntervalHandle(IntervalHandle&&) = delete;
  IntervalHandle& operator=(IntervalHandle&&) = delete;

 public:
  IntervalHandle(void* handle, IntervalCallback callback) : handle(handle), callback(callback) {}

  void cancel();

  bool isRunning();

  friend void intervalCb(uv_timer_t* uvHandle);
};

std::shared_ptr<IntervalHandle> uvIntervalStart(uint64_t intervalMs, uint64_t delayMs, IntervalCallback cb,
                                                uv_loop_t* loop = nullptr);
}  // namespace uvpp
