#include <asyncpp_uv/asyncpp_uv.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

int main() {
  printf("start\n");
  auto handle1 = asyncpp_uv::wrapper::uvTimerStart(100, []() { printf("done1\n"); });
  auto handle2 = asyncpp_uv::wrapper::uvTimerStart(1000, []() { printf("done2\n"); });
  auto handle3 = asyncpp_uv::wrapper::uvTimerStart(500, [handle1, handle2]() {
    printf("done3\n");
    handle1->cancel();
    handle2->cancel();

    asyncpp_uv::wrapper::uvTimerStart(500, []() { printf("done4\n"); });
  });

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
