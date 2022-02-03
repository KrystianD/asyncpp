#include <asyncpp/asyncpp.h>
#include <asyncpp_uv_curl/asyncpp_uv_curl.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;

task<void> testGet() {
  auto resp = co_await asyncpp_uv_curl::uvCurlGET("https://httpbin.org/get");
  printf("%s\n", resp.toString().c_str());
}

task<void> testPost() {
  auto resp = co_await asyncpp_uv_curl::uvCurlPOST("https://httpbin.org/post", "key=val");
  printf("%s\n", resp.toString().c_str());
}

task<void> testDelete() {
  auto resp = co_await asyncpp_uv_curl::uvCurlDELETE("https://httpbin.org/delete", "key=val");
  printf("%s\n", resp.toString().c_str());
}

task<void> testCancel() {
  curl_uv::CurlRequest req = curl_uv::get("https://httpbin.org/delay/5");
  cancellation_token_source tcs;

  uvpp::uvTimerStart(1000, [&tcs]() { tcs.cancel(); });
  try {
    co_await asyncpp_uv_curl::uvCurlExecute(std::move(req), tcs.get_token());
  } catch (task_cancelled&) {
    printf("curl cancelled\n");
  }
}

task<void> asyncMain() {
  printf("asyncMain BEGIN\n");
  co_await testGet();
  co_await testPost();
  co_await testDelete();
  co_await testCancel();
  printf("asyncMain END\n");
}

int main() {
  asyncMain();
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  curl_uv::unload();
  return 0;
}
