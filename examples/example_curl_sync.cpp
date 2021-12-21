#include <asyncpp_uv_curl/curl_uv.h>

using namespace std;
using namespace curl_uv;

void testGet() {
  CurlRequest req = get("https://httpbin.org/get");
  CurlResponse resp = executeSync(std::move(req));
  printf("%s\n", resp.to_string().c_str());
}

void testPost() {
  CurlRequest req = post("https://httpbin.org/post");
  req.setPayload("key=val");
  CurlResponse resp = executeSync(std::move(req));
  printf("%s\n", resp.to_string().c_str());
}

void testDelete() {
  CurlRequest req = delete_("https://httpbin.org/delete");
  req.setPayload("key=val");
  CurlResponse resp = executeSync(std::move(req));
  printf("%s\n", resp.to_string().c_str());
}

int main() {
  testGet();
  testPost();
  testDelete();

  curl_uv::unload();
  return 0;
}
