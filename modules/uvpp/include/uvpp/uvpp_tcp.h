#pragma once

#include <uv.h>

#include <functional>
#include <string>

namespace uvpp {
using TcpConnectCb = std::function<void(int)>;

void uvTcpConnect(uv_tcp_t* tcp, const struct sockaddr* addr, TcpConnectCb cb);
void uvTcpConnect(uv_tcp_t* tcp, const std::string& ip, uint16_t port, TcpConnectCb cb);
}  // namespace uvpp
