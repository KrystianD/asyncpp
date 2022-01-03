#include <string.h>
#include <uvpp/uvpp_dns.h>

namespace uvpp {
int uvGetAddrInfo(const char* node, const char* service, const struct addrinfo* hints, GetAddrInfoCb cb,
                  uv_loop_t* loop) {
  uv_getaddrinfo_t* req = new uv_getaddrinfo_t();
  req->data = new GetAddrInfoCb(std::move(cb));

  return uv_getaddrinfo(
      loop ? loop : uv_default_loop(), req,
      [](uv_getaddrinfo_t* req, int status, struct addrinfo* res) {
        GetAddrInfoCb* cb = (GetAddrInfoCb*)req->data;

        (*cb)(req, status, res);

        if (status == 0) uv_freeaddrinfo(res);

        delete cb;
        delete req;
      },
      node, service, hints);
}

int uvResolveIPv4(const std::string& host, ResolveIPv4Cb cb, uv_loop_t* loop) {
  struct addrinfo hints {};
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = 0;
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  return uvGetAddrInfo(
      host.c_str(), nullptr, &hints,
      [cb = std::move(cb)](uv_getaddrinfo_t* req, int status, struct addrinfo* res) {
        if (status == 0) {
          char addr[INET_ADDRSTRLEN];

          std::vector<std::string> ipAddresses;
          for (struct addrinfo* tmp = res; tmp != nullptr; tmp = tmp->ai_next) {
            struct in_addr* ptr = &((struct sockaddr_in*)tmp->ai_addr)->sin_addr;

            inet_ntop(tmp->ai_family, ptr, addr, INET_ADDRSTRLEN);

            ipAddresses.emplace_back(addr);
          }
          cb(0, ipAddresses);
        } else {
          cb(status, {});
        }
      },
      loop);
}
}  // namespace uvpp
