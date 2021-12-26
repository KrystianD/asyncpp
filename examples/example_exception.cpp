#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv_sleep.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

task<int> raise() {
  co_await uvSleep(100);
  throw std::runtime_error("error");
  co_return 2;
}

task<void> asyncMain() {
  try {
    co_await raise();
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
