#include <uv.h>

#include <uvpp/uvpp.h>

using namespace std;

int main() {
  printf("start\n");
  auto interval = uvpp::uvIntervalStart(500, 0, []() { printf("tick\n"); });

  uvpp::uvTimerStart(2000, [interval]() {
    printf("cancel\n");
    interval->cancel();
  });

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
