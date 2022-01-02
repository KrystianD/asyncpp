#include <uvpp/uvpp_stream.h>

namespace uvpp {
void uvShutdown(uv_stream_t* stream, ShutdownCb cb) {
  uv_shutdown_t* req = new uv_shutdown_t();
  req->data = new ShutdownCb(std::move(cb));

  uv_shutdown(req, stream, [](uv_shutdown_t* reqInner, int status) {
    ShutdownCb* data = (ShutdownCb*)reqInner->data;
    (*data)(status);
    delete data;

    delete reqInner;
  });
}

void uvWrite(uv_stream_t* stream, const void* data, size_t length, WriteCb cb) {
  uv_buf_t b[] = {
      {.base = (char*)data, .len = length},
  };

  uv_write_t* req = new uv_write_t();
  req->data = new WriteCb(std::move(cb));

  uv_write(req, stream, b, 1, [](uv_write_t* req, int status) {
    WriteCb* cb = (WriteCb*)req->data;
    (*cb)(status);
    delete cb;

    delete req;
  });
}
}  // namespace uvpp
