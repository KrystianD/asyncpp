#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv_sleep.h>
#include <asyncpp_uv/asyncpp_uv_timer.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

task<int> raise1() {
  co_await uvSleep(100);
  throw std::runtime_error("error1");
}

task<int> raise2() {
  throw std::runtime_error("error2");
  co_await uvSleep(100);
}

task<int> raise3() {
  return asyncpp::makeTask<int>([](auto resolve, auto reject) {
    asyncpp_uv::timer::uvTimerStart(100, [resolve, reject]() { reject(std::logic_error("error3")); });
  });
}

task<int> raise4() {
  return asyncpp::makeTask<int>([](auto resolve, auto reject) { reject(std::logic_error("error4")); });
}

task<int> raise5() { throw std::runtime_error("error5"); }

task<void> asyncMain() {
  try {
    co_await raise1();
  } catch (std::exception &e) {
    printf("EXCEPTION: %s\n", e.what());
  }

  try {
    co_await raise2();
  } catch (std::exception &e) {
    printf("EXCEPTION: %s\n", e.what());
  }

  try {
    co_await raise3();
  } catch (std::exception &e) {
    printf("EXCEPTION: %s\n", e.what());
  }

  try {
    co_await raise4();
  } catch (std::exception &e) {
    printf("EXCEPTION: %s\n", e.what());
  }

  try {
    co_await raise5();
  } catch (std::exception &e) {
    printf("EXCEPTION: %s\n", e.what());
  }

  co_return;
}

int main() {
  asyncMain();
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
