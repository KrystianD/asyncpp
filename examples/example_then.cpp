#include <asyncpp_uv/asyncpp_uv_sleep.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

task<int> test() { co_return 5; }

int main() {
  uvSleep(1000).then([]() { printf("then1\n"); });

  test().then([](int x) { printf("val %d\n", x); });

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
