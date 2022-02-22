#include <uv.h>

#include <thread>

#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv.h>
#include <uvpp/uvpp.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

int getThreadId() { return gettid(); }

void callback() { printf("callback (thread: %d)\n", getThreadId()); }

int main() {
  printf("main (thread: %d)\n", getThreadId());

  auto executor = uvpp::utils::uvExecutor::create(10);

  std::thread th([executor]() {
    for (int i = 0; i < 3; i++) {
      usleep(100 * 1000);

      printf("loop (thread: %d)\n", getThreadId());

      executor->execute([]() { callback(); });
    }
  });

  asyncpp::run([]() -> task<void> { co_await uvSleepAsync(1000ms); });

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  th.join();

  return 0;
}
