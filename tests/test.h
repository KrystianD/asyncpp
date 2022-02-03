#pragma once

#include <asyncpp/asyncpp.h>
#include <gtest/gtest.h>
#include <uv.h>

#include <thread>
#include <pthread.h>

static sem_t sem;

static void runAsync(const std::function<asyncpp::task<void>()> &f) {
  sem_init(&sem, 0, 0);

  std::exception_ptr excPtr;
  f().then([]() { sem_post(&sem); },
           [&excPtr](const std::exception_ptr &exc) {
             excPtr = exc;
             sem_post(&sem);
           });
  sem_wait(&sem);
  if (excPtr) std::rethrow_exception(excPtr);
}

static void EXPECT_TIME_ASYNC(int timeMs, const std::function<asyncpp::task<void>()> &f) {
  auto start = std::chrono::high_resolution_clock::now();
  runAsync(f);
  auto finish = std::chrono::high_resolution_clock::now();

  int ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

  EXPECT_NEAR(ms, timeMs, 20);
}