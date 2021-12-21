#include <asyncpp_uv/asyncpp_uv_wrapper.h>
#include <uv.h>

#include <functional>

namespace asyncpp_uv::wrapper {
void uvCloseDelete(void* handle) {
  uv_close((uv_handle_t*)handle, [](uv_handle_t* handleInner) { delete handleInner; });
}

void uvTimerStart(uv_loop_t* loop, uint64_t timeoutMs, const TimerCallback& cb) {
  uv_timer_t* handle = new uv_timer_t();
  handle->data = new TimerCallback(cb);

  uv_timer_init(loop, handle);
  uv_timer_start(
      handle,
      [](uv_timer_t* handleInner) {
        TimerCallback* data = (TimerCallback*)handleInner->data;
        (*data)();
        uv_timer_stop(handleInner);
        delete data;
        uvCloseDelete(handleInner);
      },
      timeoutMs, 0);
}
}  // namespace asyncpp_uv::wrapper
