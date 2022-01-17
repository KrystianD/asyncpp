#include <uv_tcptls/uv_tcptls.h>
#include <uvpp/uvpp.h>

#include <openssl/x509v3.h>
#include <cstring>

using namespace std;

int main() {
  signal(SIGPIPE, SIG_IGN);

  uv_tcptls_t* tcptls = new uv_tcptls_t();

  SSL_CTX* ssl_ctx = SSL_CTX_new(TLS_client_method());

  if (SSL_CTX_set_min_proto_version(ssl_ctx, TLS1_2_VERSION) != 1) {
    exit(EXIT_FAILURE);
  }

  if (SSL_CTX_set_default_verify_paths(ssl_ctx) != 1) {
    exit(EXIT_FAILURE);
  }

  uv_tcptls_init(uv_default_loop(), tcptls, ssl_ctx);

  const char* hostname = "httpbin.org";
  int port = 443;

  uv_tcptls_set_setupssl_cb(
      tcptls,
      [](uv_tcptls_t* handle, SSL* ssl, void* userdata) {
        const char* hostname = (char*)userdata;
        SSL_set_tlsext_host_name(ssl, hostname);
        SSL_set_verify(ssl, SSL_VERIFY_PEER, NULL);
        SSL_set_hostflags(ssl, X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
        SSL_set1_host(ssl, hostname);
      },
      (void*)hostname);

  uv_tcptls_read_start(tcptls, [](uv_tcptls_t* handle, ssize_t nread, const void* buf) {
    if (nread < 0) {
      printf("--- connection closed\n");
      uv_tcptls_close(handle, [](uv_tcptls_t* tcptls) { delete tcptls; });
    } else {
      printf("%.*s", (int)nread, (char*)buf);
    }
  });

  uvpp::uvResolveIPv4(hostname, [tcptls, port](int status, const std::vector<std::string>& ipAddresses) {
    struct sockaddr_in server;
    memset((char*)&server, 0, sizeof(server));

    string ip = ipAddresses[0];

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &(server.sin_addr));
    printf("--- IP %s\n", ip.c_str());

    uv_tcptls_connect(tcptls, (sockaddr*)&server, [](uv_tcptls_t* handle, int result) {
      printf("--- connect result: %d\n", result);

      if (result == 0) {
        const char* req = "GET /get HTTP/1.1\nHost: httpbin.org\nConnection: close\n\n";
        uv_tcptls_try_write(handle, req, strlen(req));
      } else {
        uv_tcptls_close(handle, [](uv_tcptls_t* tcptls) { delete tcptls; });
      }
    });
  });

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  SSL_CTX_free(ssl_ctx);

  return 0;
}
