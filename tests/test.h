#pragma once

#include <asyncpp/asyncpp.h>
#include <gtest/gtest.h>
#include <uv.h>

#include <semaphore>
#include <thread>

static std::binary_semaphore sem(false);

static void runAsync(const std::function<asyncpp::task<void>()> &f) {
  std::exception_ptr excPtr;
  f().then([]() { sem.release(); },
           [&excPtr](const std::exception_ptr &exc) {
             excPtr = exc;
             sem.release();
           });
  sem.acquire();
  if (excPtr) std::rethrow_exception(excPtr);
}

static void EXPECT_TIME_ASYNC(int timeMs, const std::function<asyncpp::task<void>()> &f) {
  auto start = std::chrono::high_resolution_clock::now();
  runAsync(f);
  auto finish = std::chrono::high_resolution_clock::now();

  int ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

  EXPECT_NEAR(ms, timeMs, 20);
}