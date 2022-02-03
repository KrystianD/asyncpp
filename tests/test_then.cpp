#include <asyncpp_uv/asyncpp_uv.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

#include "obj.h"

using obj::Obj;

// void
task<void> test_void_ok() {
  co_await uvSleepAsync(1000);
  co_return;
}

bool test_void_fail_early_failed = false;
task<void> test_void_fail_early() {
  throw std::runtime_error("err");
  co_await uvSleepAsync(1000);
}

bool test_void_fail_late_failed = false;
task<void> test_void_fail_late() {
  co_await uvSleepAsync(1000);
  throw std::runtime_error("err");
}

// obj
task<Obj> test_obj_ok() {
  co_await uvSleepAsync(1000);
  co_return Obj(5);
}

bool test_obj_fail_early_failed = false;
task<Obj> test_obj_fail_early() {
  throw std::runtime_error("err");
  co_await uvSleepAsync(1000);
}

bool test_obj_fail_late_failed = false;
task<Obj> test_obj_fail_late() {
  co_await uvSleepAsync(1000);
  throw std::runtime_error("err");
}

void run() {
  test_void_ok().then([]() { printf("test_void_ok\n"); });

  test_void_fail_early().then([]() { printf("test_void_fail_early\n"); },
                              [](std::exception_ptr ptr) {
                                try {
                                  std::rethrow_exception(ptr);
                                } catch (std::runtime_error& e) {
                                  test_void_fail_early_failed = true;
                                }
                              });

  test_void_fail_late().then([]() { printf("test_void_fail_late\n"); },
                             [](std::exception_ptr ptr) {
                               try {
                                 std::rethrow_exception(ptr);
                               } catch (std::runtime_error& e) {
                                 test_void_fail_late_failed = true;
                               }
                             });

  test_obj_ok().then([](Obj o) { printf("test_obj_ok %d\n", o.v); });

  test_obj_fail_early().then([](Obj o) { printf("test_obj_fail_early %d\n", o.v); },
                             [](std::exception_ptr ptr) {
                               try {
                                 std::rethrow_exception(ptr);
                               } catch (std::runtime_error& e) {
                                 test_obj_fail_early_failed = true;
                               }
                             });

  test_obj_fail_late().then([](Obj o) { printf("test_obj_fail_late %d\n", o.v); },
                            [](std::exception_ptr ptr) {
                              try {
                                std::rethrow_exception(ptr);
                              } catch (std::runtime_error& e) {
                                test_obj_fail_late_failed = true;
                              }
                            });
}

int main() {
  run();

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  printf("test_void_fail_early_failed: %d\n", test_void_fail_early_failed);
  printf("test_void_fail_late_failed: %d\n", test_void_fail_late_failed);
  printf("test_obj_fail_early_failed: %d\n", test_obj_fail_early_failed);
  printf("test_obj_fail_late_failed: %d\n", test_obj_fail_late_failed);
  printf("copies: %d\n", obj::copies);
  printf("moves: %d\n", obj::moves);

  if (!test_void_fail_early_failed) return 1;
  if (!test_void_fail_late_failed) return 1;
  if (!test_obj_fail_early_failed) return 1;
  if (!test_obj_fail_late_failed) return 1;
  if (obj::copies > 0) return 1;

  return 0;
}
