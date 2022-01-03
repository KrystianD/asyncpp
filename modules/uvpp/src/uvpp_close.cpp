#include <uvpp/uvpp_close.h>

namespace uvpp {
void uvClose(uv_handle_t* handle, CloseCb cb) {
  handle->data = new CloseCb(std::move(cb));

  uv_close((uv_handle_t*)handle, [](uv_handle_t* handleInner) {
    CloseCb* cb = (CloseCb*)handleInner->data;
    (*cb)(handleInner);

    delete cb;
  });
}
}  // namespace uvpp
