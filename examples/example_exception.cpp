#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv_sleep.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

task<void> raise() {
  co_await uvSleep(100);
  throw std::runtime_error("test error");
}

task<void> catchInCoroutine() {
  try {
    co_await raise();
  } catch (const std::exception& e) {
    printf("EXCEPTION: %s\n", e.what());
  }
}

task<void> catchInThen() {
  raise().then([]() { printf("completed\n"); },
               [](const std::exception_ptr& exc) {
                 try {
                   rethrow_exception(exc);
                 } catch (const std::runtime_error& e) {
                   printf("EXCEPTION: %s\n", e.what());
                 }
               });
  co_return ;
}

task<void> asyncMain() {
  co_await catchInCoroutine();
  co_await catchInThen();

  co_return;
}

int main() {
  asyncMain();
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
