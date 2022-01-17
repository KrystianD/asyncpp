#include "uv_tcptls/uv_tcptls.h"

#include <malloc.h>
#include <string.h>

#include <openssl/err.h>

static void uv_tcptls_do_handshake(uv_tcptls_t* handle);
static void uv_tcptls_on_handshake_data(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
static void uv_tcptls_on_data(uv_stream_t* stream, ssize_t nread, const uv_buf_t* uv_buf);
static void uv_tcptls_flush_read(uv_tcptls_t* handle, char* tempBuf, int tempBufSize);

static void uv_tcptls_on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  uv_tcptls_t* state = (uv_tcptls_t*)handle->data;

  if (state->buffer_size < suggested_size) {
    free(state->buffer);
    state->buffer_size = (int)suggested_size;
    state->buffer = malloc(state->buffer_size);
  }

  buf->base = (char*)state->buffer;
  buf->len = state->buffer_size;
}

static void uv_tcptls_on_connect(uv_connect_t* req, int status) {
  uv_tcptls_t* handle = (uv_tcptls_t*)req->data;
  free(req);

  if (status != 0) {
    handle->connect_cb(handle, status);
    return;
  }

  int fd;
  uv_fileno((uv_handle_t*)handle->tcp, &fd);
  BIO* in_bio = BIO_new(BIO_s_mem());
  BIO* out_bio = BIO_new_fd(fd, 0);
  SSL_set_bio(handle->ssl, in_bio, out_bio);

  SSL_set_connect_state(handle->ssl);

  if (handle->setupssl_cb) handle->setupssl_cb(handle, handle->ssl, handle->setupssl_cb_userdata);

  uv_tcptls_do_handshake(handle);
}

static void uv_tcptls_do_handshake(uv_tcptls_t* handle) {
  int rc = SSL_do_handshake(handle->ssl);

  uv_read_stop((uv_stream_t*)handle->tcp);

  if (rc == 1) {
    handle->established = 1;
    handle->connect_cb(handle, 0);

    if (handle->read_cb) {
      uv_read_start((uv_stream_t*)handle->tcp, uv_tcptls_on_alloc, uv_tcptls_on_data);
    }
  } else {
    int res = SSL_get_error(handle->ssl, rc);

    if (res == SSL_ERROR_WANT_READ) {
      uv_read_start((uv_stream_t*)handle->tcp, uv_tcptls_on_alloc, uv_tcptls_on_handshake_data);
    } else {
      handle->connect_cb(handle, EHANDSHAKE);
    }
  }
}

static void uv_tcptls_on_handshake_data(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  uv_tcptls_t* handle = (uv_tcptls_t*)stream->data;

  if (nread < 0) {
    handle->connect_cb(handle, EHANDSHAKE);
    return;
  }

  BIO_write(SSL_get_rbio(handle->ssl), buf->base, nread);
  uv_tcptls_do_handshake(handle);
}

static void uv_tcptls_on_data(uv_stream_t* stream, ssize_t nread, const uv_buf_t* uv_buf) {
  uv_tcptls_t* handle = (uv_tcptls_t*)stream->data;
  char* buf = (char*)uv_buf->base;

  if (nread < 0) {
    handle->read_cb(handle, nread, 0);
    return;
  }

  size_t len = nread;
  size_t free_space = 0;

  char* pbuf = buf;

  while (len > 0) {
    size_t written;
    int rc = BIO_write_ex(SSL_get_rbio(handle->ssl), pbuf, (int)len, &written);

    if (rc == 1) {
      len -= written;
      pbuf += written;
      free_space += written;

      // reuse the incoming buffer up to already read bytes
      uv_tcptls_flush_read(handle, buf, free_space);
    } else {
      // int err = uv_tcptls_get_ssl_error(handle->ssl, rc);

      handle->read_cb(handle, ESSL, 0);
      uv_read_stop((uv_stream_t*)handle->tcp);
      return;
    }
  }
}

static void uv_tcptls_flush_read(uv_tcptls_t* handle, char* tempBuf, int tempBufSize) {
  int rc, err;

  do {
    size_t read = -1;
    rc = SSL_read_ex(handle->ssl, (void*)tempBuf, (int)tempBufSize, &read);

    if (rc == 1) {
      handle->read_cb(handle, (ssize_t)read, tempBuf);
      if (handle->read_cb == NULL) {
        return;
      }
    } else {
      err = SSL_get_error(handle->ssl, rc);

      if (err == SSL_ERROR_WANT_READ) {
        break;
      } else {
        handle->read_cb(handle, ESSL, 0);
        uv_read_stop((uv_stream_t*)handle->tcp);
        return;
      }
    }
  } while (1);
}

static void uv_tcptls_free(uv_tcptls_t* handle) {
  if (handle->tcp) free(handle->tcp);
  if (handle->buffer) free(handle->buffer);

  if (handle->ssl) SSL_free(handle->ssl);

  handle->tcp = NULL;
  handle->buffer = NULL;
  handle->ssl = NULL;
}

static void uv_tcptls_on_close(uv_handle_t* uv_handle) {
  uv_tcptls_t* handle = (uv_tcptls_t*)uv_handle->data;
  uv_tcptls_free(handle);
  handle->close_cb(handle);
}

// Public API
void uv_tcptls_init(uv_loop_t* loop, uv_tcptls_t* handle, SSL_CTX* ssl_ctx) {
  memset(handle, 0, sizeof(uv_tcptls_t));

  handle->tcp = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
  handle->tcp->data = handle;
  handle->ssl = SSL_new(ssl_ctx);
  handle->buffer_size = 100 * 1024;
  handle->buffer = malloc(handle->buffer_size);
  handle->established = 0;

  handle->setupssl_cb = NULL;
  handle->connect_cb = NULL;
  handle->close_cb = NULL;
  handle->read_cb = NULL;

  uv_tcp_init(loop, handle->tcp);
}

void uv_tcptls_set_setupssl_cb(uv_tcptls_t* handle, uv_tcptls_setupssl_cb setupssl_cb, void* userdata) {
  handle->setupssl_cb = setupssl_cb;
  handle->setupssl_cb_userdata = userdata;
}

void uv_tcptls_nodelay(uv_tcptls_t* handle, int enable) { uv_tcp_nodelay(handle->tcp, enable); }
void uv_tcptls_keepalive(uv_tcptls_t* handle, int enable, unsigned int delay) {
  uv_tcp_keepalive(handle->tcp, enable, delay);
}

void uv_tcptls_close(uv_tcptls_t* handle, uv_tcptls_close_cb close_cb) {
  uv_read_stop((uv_stream_t*)handle->tcp);

  SSL_shutdown(handle->ssl);

  handle->close_cb = close_cb;
  uv_close((uv_handle_t*)handle->tcp, uv_tcptls_on_close);
}

void uv_tcptls_connect(uv_tcptls_t* handle, const struct sockaddr* addr, uv_tcptls_connect_cb cb) {
  uv_connect_t* req = (uv_connect_t*)malloc(sizeof(uv_connect_t));
  req->data = handle;

  handle->connect_cb = cb;

  uv_tcp_connect(req, handle->tcp, addr, uv_tcptls_on_connect);
}

void uv_tcptls_read_start(uv_tcptls_t* handle, uv_tcptls_read_cb read_cb) {
  handle->read_cb = read_cb;

  if (handle->established) {
    uv_read_start((uv_stream_t*)handle->tcp, uv_tcptls_on_alloc, uv_tcptls_on_data);

    uv_tcptls_flush_read(handle, handle->buffer, handle->buffer_size);
  }
}

void uv_tcptls_read_stop(uv_tcptls_t* handle) {
  handle->read_cb = 0;

  if (handle->established) {
    uv_read_stop((uv_stream_t*)handle->tcp);
  }
}

int uv_tcptls_try_write(uv_tcptls_t* handle, const void* data, size_t length) {
  if (handle->established) {
    size_t written = 0;
    int rc = SSL_write_ex(handle->ssl, data, length, &written);
    if (rc <= 0) {
      int err = SSL_get_error(handle->ssl, rc);
      if (err == SSL_ERROR_WANT_WRITE) {
        return UV_EAGAIN;
      } else {
        return UV_EIO;
      }
    } else {
      return written;
    }
  } else {
    return UV_EIO;
  }
}