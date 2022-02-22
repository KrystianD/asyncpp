#include <uv.h>

#include <thread>

#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv.h>
#include <uvpp/uvpp.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

void callback() { printf("callback\n"); }

int main() {
  asyncpp::run([]() -> task<void> {
    co_await uvSleepAsync(1000ms);
    uvpp::uvAsyncSend([]() { callback(); });
  });

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  return 0;
}
