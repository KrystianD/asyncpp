#include <asyncpp/asyncpp.h>
#include <asyncpp_uv_curl/asyncpp_uv_curl.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;

task<void> testGet() {
  auto resp = co_await asyncpp_uv_curl::uvCurlGET("https://httpbin.org/get");
  printf("%s\n", resp.to_string().c_str());
}

task<void> testPost() {
  auto resp = co_await asyncpp_uv_curl::uvCurlPOST("https://httpbin.org/post", "key=val");
  printf("%s\n", resp.to_string().c_str());
}

task<void> testDelete() {
  auto resp = co_await asyncpp_uv_curl::uvCurlDELETE("https://httpbin.org/delete", "key=val");
  printf("%s\n", resp.to_string().c_str());
}

task<void> asyncMain() {
  printf("asyncMain BEGIN\n");
  co_await testGet();
  co_await testPost();
  co_await testDelete();
  printf("asyncMain END\n");
}

int main() {
  asyncMain();
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  curl_uv::unload();
  return 0;
}
