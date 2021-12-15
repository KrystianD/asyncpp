#include <asyncpp_uv/asyncpp_uv_sleep.h>
#include <uv.h>

asyncpp::task<void> asyncMain() {
  printf("asyncMain BEGIN\n");
  auto timer1 = asyncpp_uv::uvSleep(1000);
  auto timer2 = asyncpp_uv::uvSleep(1000);
  co_await timer1;
  co_await timer2;
  printf("asyncMain END\n");
}

int main() {
  asyncMain();
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}