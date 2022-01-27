#pragma once

#include <uv.h>

#include <chrono>
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

inline std::shared_ptr<IntervalHandle> uvIntervalStart(const std::chrono::milliseconds& interval,
                                                       const std::chrono::milliseconds& delay, IntervalCallback cb,
                                                       uv_loop_t* loop = nullptr) {
  return uvIntervalStart((uint64_t)interval.count(), (uint64_t)delay.count(), cb, loop);
}
}  // namespace uvpp
