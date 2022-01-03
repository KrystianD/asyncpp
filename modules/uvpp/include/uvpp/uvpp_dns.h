#pragma once

#include <uv.h>

#include <cstdint>
#include <functional>
#include <memory>

namespace uvpp {
// uv_getaddrinfo
using GetAddrInfoCb = std::function<void(uv_getaddrinfo_t* req, int status, struct addrinfo* res)>;

int uvGetAddrInfo(const char* node, const char* service, const struct addrinfo* hints, GetAddrInfoCb cb,
                  uv_loop_t* loop = nullptr);

// IPv4 helper for uv_getaddrinfo
using ResolveIPv4Cb = std::function<void(int status, const std::vector<std::string>& ipAddresses)>;

int uvResolveIPv4(const std::string& host, ResolveIPv4Cb cb, uv_loop_t* loop = nullptr);
}  // namespace uvpp
