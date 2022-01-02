#include <uvpp/uvpp_tcp.h>

namespace uvpp {
void uvTcpConnect(uv_tcp_t* tcp, const struct sockaddr* addr, TcpConnectCb cb) {
  uv_connect_t* req = new uv_connect_t();
  req->data = new TcpConnectCb(cb);

  uv_tcp_connect(req, tcp, addr, [](uv_connect_t* reqInner, int status) {
    TcpConnectCb* data = (TcpConnectCb*)reqInner->data;
    (*data)(status);
    delete data;

    delete reqInner;
  });
}

void uvTcpConnect(uv_tcp_t* tcp, const std::string& ip, uint16_t port, TcpConnectCb cb) {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

  uvTcpConnect(tcp, (sockaddr*)&addr, cb);
}
}  // namespace uvpp
