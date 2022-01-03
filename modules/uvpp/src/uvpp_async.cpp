#include <uvpp/uvpp_async.h>

namespace uvpp {
void uvAsyncSend(AsyncCallback cb, uv_loop_t* loop) {
  uv_async_t* async = new uv_async_t();
  async->data = new AsyncCallback(std::move(cb));
  uv_async_init(loop ? loop : uv_default_loop(), async, [](uv_async_t* handle) {
    AsyncCallback* cb = (AsyncCallback*)handle->data;

    (*cb)();

    delete cb;

    uv_close((uv_handle_t*)handle, [](uv_handle_t* handle_) { delete (uv_async_t*)handle_; });
  });
  uv_async_send(async);
}
}  // namespace uvpp
