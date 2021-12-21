#include <asyncpp_uv/asyncpp_uv_timer.h>
#include <uv.h>

#include <functional>

namespace asyncpp_uv::timer {
void uvCloseDelete(void* handle) {
  uv_close((uv_handle_t*)handle, [](uv_handle_t* handleInner) { delete handleInner; });
}

using TimerDataHandleSPtr = std::shared_ptr<TimerHandle>;

void TimerHandle::cancel() {
  if (this->handle == nullptr) return;

  uv_timer_t* uvHandle = (uv_timer_t*)this->handle;

  TimerDataHandleSPtr* timerHandleInner = (TimerDataHandleSPtr*)uvHandle->data;
  delete timerHandleInner;

  uv_timer_stop(uvHandle);
  uvCloseDelete(uvHandle);

  this->handle = nullptr;
}

std::shared_ptr<TimerHandle> uvTimerStart(uint64_t timeoutMs, const TimerCallback& cb, uv_loop_t* loop) {
  if (loop == nullptr) loop = uv_default_loop();

  uv_timer_t* uvHandle = new uv_timer_t();

  std::shared_ptr<TimerHandle> timerHandle = std::make_shared<TimerHandle>(uvHandle, cb);

  uvHandle->data = new TimerDataHandleSPtr(timerHandle);

  uv_timer_init(loop, uvHandle);
  uv_timer_start(
      uvHandle,
      [](uv_timer_t* uvHandleInner) {
        TimerDataHandleSPtr* timerHandleInner = (TimerDataHandleSPtr*)uvHandleInner->data;
        (*timerHandleInner)->callback();
        (*timerHandleInner)->handle = nullptr;
        delete timerHandleInner;

        uv_timer_stop(uvHandleInner);
        uvCloseDelete(uvHandleInner);
      },
      timeoutMs, 0);

  return timerHandle;
}
}  // namespace asyncpp_uv::timer
