#include <uv.h>

#include <asyncpp_uv/asyncpp_uv.h>
#include <uvpp/uvpp.h>

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

long expensiveWork() {
  long v = 0;
  usleep(1 * 1000 * 1000);
  return v;
}

int main() {
  //  uvpp::uvRunInThread<long>(expensiveWork, [](long res) { printf("res1 %ld\n", res); });
  //  uvpp::uvRunInThread<long>(expensiveWork, [](long res) { printf("res2 %ld\n", res); });
  //  uvpp::uvRunInThread<long>(expensiveWork, [](long res) { printf("res3 %ld\n", res); });

  asyncpp::run([]() -> task<void> {
    task<long> res1_task = asyncpp_uv::uvRunInThreadAsync<long>([]() { return expensiveWork(); });
    task<long> res2_task = asyncpp_uv::uvRunInThreadAsync<long>([]() { return expensiveWork(); });
    task<long> res3_task = asyncpp_uv::uvRunInThreadAsync<long>([]() { return expensiveWork(); });

    long res1 = co_await res1_task;
    long res2 = co_await res2_task;
    long res3 = co_await res3_task;

    printf("res1 %lu\n", res1);
    printf("res2 %lu\n", res2);
    printf("res3 %lu\n", res3);
  });

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
