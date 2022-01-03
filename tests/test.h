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