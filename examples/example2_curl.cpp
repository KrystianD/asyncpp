#include <asyncpp/asyncpp.h>
#include <asyncpp_uv_curl/asyncpp_uv_curl.h>
#include <uv.h>

using namespace std;
using namespace asyncpp;

task<void> asyncMain() {
  printf("asyncMain BEGIN\n");
  auto resp = co_await asyncpp_uv_curl::uvCurlGET("https://httpbin.org/get");
  printf("%s\n", resp.to_string().c_str());
  printf("asyncMain END\n");
}

int main() {
  asyncMain();
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  curl_uv::unload();
  return 0;
}
