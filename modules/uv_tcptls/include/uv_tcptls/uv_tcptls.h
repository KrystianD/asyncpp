#pragma once

#include <openssl/ssl.h>
#include <uv.h>

#ifdef __cplusplus
extern "C" {
#endif

struct uv_tcptls_s;

typedef struct uv_tcptls_s uv_tcptls_t;

typedef void (*uv_tcptls_setupssl_cb)(uv_tcptls_t* handle, SSL* ssl, void* userdata);
typedef void (*uv_tcptls_connect_cb)(uv_tcptls_t* handle, int result);
typedef void (*uv_tcptls_close_cb)(uv_tcptls_t* handle);
typedef void (*uv_tcptls_read_cb)(uv_tcptls_t* handle, ssize_t nread, const void* buf);

typedef enum {
  EHANDSHAKE = UV_ERRNO_MAX - 1,
  ESSL = UV_ERRNO_MAX - 2,
} uv_tcptls_errno_t;

struct uv_tcptls_s {
  void* data;

  uv_tcp_t* tcp;

  uv_tcptls_setupssl_cb setupssl_cb;
  void* setupssl_cb_userdata;
  uv_tcptls_connect_cb connect_cb;
  uv_tcptls_close_cb close_cb;
  uv_tcptls_read_cb read_cb;

  void* buffer;
  int buffer_size;

  int established;

  SSL* ssl;
};

void uv_tcptls_init(uv_loop_t* loop, uv_tcptls_t* handle, SSL_CTX* ssl_ctx);

void uv_tcptls_set_setupssl_cb(uv_tcptls_t* handle, uv_tcptls_setupssl_cb setupssl_cb, void* userdata);
void uv_tcptls_nodelay(uv_tcptls_t* handle, int enable);
void uv_tcptls_keepalive(uv_tcptls_t* handle, int enable, unsigned int delay);

void uv_tcptls_connect(uv_tcptls_t* handle, const struct sockaddr* addr, uv_tcptls_connect_cb cb);

void uv_tcptls_close(uv_tcptls_t* handle, uv_tcptls_close_cb close_cb);

void uv_tcptls_read_start(uv_tcptls_t* handle, uv_tcptls_read_cb read_cb);
void uv_tcptls_read_stop(uv_tcptls_t* handle);

int uv_tcptls_try_write(uv_tcptls_t* handle, const void* data, size_t length);

#ifdef __cplusplus
}
#endif