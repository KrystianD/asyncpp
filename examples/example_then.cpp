#include <asyncpp_uv/asyncpp_uv_sleep.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

task<int> test() {
  co_await uvSleep(1000);
  co_return 5;
}

void run() {
  test().then([](int r) { printf("then1 %d\n", r); });
}

int main() {
  run();

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
