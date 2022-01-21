#include <uv.h>

#include <thread>

#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv.h>
#include <uvpp/uvpp.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

int getThreadId() { return gettid(); }

void callback(int v) { printf("callback %d (thread: %d)\n", v, getThreadId()); }

int main() {
  auto channel = uvpp::uvChannel<int>::create(5, [](int v) { callback(v); });

  printf("main (thread: %d)\n", getThreadId());

  std::thread th([channel]() {
    for (int i = 0; i < 10; i++) {
      printf("loop %d (thread: %d)\n", i, getThreadId());
      channel->send(i);
    }
    usleep(100 * 1000);
    for (int i = 10; i < 20; i++) {
      printf("loop %d (thread: %d)\n", i, getThreadId());
      channel->send(i);
    }
  });

  asyncpp::run([&channel]() -> task<void> {
    co_await uvSleepAsync(1000ms);
    channel.reset();
  });

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  th.join();

  return 0;
}
