#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

task<void> asyncMain() {
  printf("asyncMain BEGIN\n");

  cancellation_token_source tcs;

  uvpp::uvTimerStart(1s, [tcs]() { tcs.cancel(); });

  try {
    auto s1 = uvSleepAsync(10s, tcs.get_token());
    auto s2 = uvSleepAsync(5s, tcs.get_token());

    co_await s1;
    co_await s2;

  } catch (task_cancelled&) {
    printf("sleep cancelled\n");
  }

  printf("asyncMain END\n");
}

int main() {
  asyncMain();
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
