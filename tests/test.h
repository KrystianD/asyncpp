#pragma once

#include <asyncpp/asyncpp.h>
#include <gtest/gtest.h>
#include <uv.h>

#include <semaphore>
#include <thread>

std::binary_semaphore sem(false);

void runAsync(const std::function<asyncpp::task<void>()> &f) {
  std::exception_ptr excPtr;
  f().then([]() { sem.release(); },
           [&excPtr](const std::exception_ptr &exc) {
             excPtr = exc;
             sem.release();
           });
  sem.acquire();
  if (excPtr) std::rethrow_exception(excPtr);
}

int main(int argc, char *argv[]) {
  bool stop = false;
  std::thread th([&stop]() {
    while (!stop) {
      int res;
      res = uv_run(uv_default_loop(), UV_RUN_ONCE);
      if (res == 0) uv_sleep(100);
    }
  });
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  stop = true;

  th.join();

  return result;
}
