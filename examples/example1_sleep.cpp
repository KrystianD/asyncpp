#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv_sleep.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

task<void> asyncMain() {
  printf("asyncMain BEGIN\n");
  co_await uvSleep(1000);
  printf("asyncMain END\n");
}

int main() {
  asyncMain();
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
