#include <asyncpp_uv/asyncpp_uv.h>
#include <uvpp/uvpp.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

int main() {
  printf("start\n");
  auto handle1 = uvpp::uvTimerStart(100, []() { printf("done1\n"); });
  auto handle2 = uvpp::uvTimerStart(1000, []() { printf("done2\n"); });
  auto handle3 = uvpp::uvTimerStart(500, [handle1, handle2]() {
    printf("done3\n");
    handle1->cancel();
    handle2->cancel();

    uvpp::uvTimerStart(500, []() { printf("done4\n"); });
  });

  auto handle4 = uvpp::uvTimerStart(1000, []() { printf("done4\n"); });
  uvpp::uvTimerStart(500, [handle4]() {
    handle4->restart();
    printf("restart\n");
  });
  uvpp::uvTimerStart(1000, [handle4]() {
    handle4->restart();
    printf("restart\n");
  });

  auto loop = [handle4]() -> task<void> {
    for (int i = 0; i < 5; i++) {
      printf("check, timer4: %d\n", handle4->isRunning());
      co_await uvSleep(800);
    }
  };

  loop();

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
