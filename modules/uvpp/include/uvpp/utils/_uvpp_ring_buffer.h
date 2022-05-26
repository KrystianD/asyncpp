#pragma once

#include <condition_variable>
#include <mutex>
#include <tuple>
#include <vector>

namespace uvpp::utils {
template<typename T>
class RingBuffer {
  std::mutex mutex;
  std::condition_variable cond;

  std::vector<T> buffer;
  size_t readPos = 0, writePos = 0;
  size_t count = 0;

 public:
  explicit RingBuffer(size_t size) { buffer.resize(size); }

  RingBuffer(RingBuffer&) noexcept = delete;
  RingBuffer& operator=(RingBuffer&) noexcept = delete;

  RingBuffer(RingBuffer&&) noexcept = default;
  RingBuffer& operator=(RingBuffer&&) noexcept = default;

  std::tuple<bool, T> tryDequeue() {
    std::unique_lock<std::mutex> lock(mutex);
    if (count > 0) {
      T value = std::move(buffer[readPos]);
      readPos++;
      if (readPos == buffer.size()) readPos = 0;
      count--;

      cond.notify_all();
      return std::make_tuple(true, std::move(value));
    } else {
      return std::make_tuple(false, T());
    }
  }

  void enqueue(T value) {
    std::unique_lock<std::mutex> lock(mutex);
    cond.wait(lock, [this]() { return count < buffer.size(); });
    buffer[writePos] = std::move(value);
    writePos++;
    if (writePos == buffer.size()) writePos = 0;
    count++;
  }
};
}  // namespace uvpp::utils