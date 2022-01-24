#include <uvpp/uvpp_interval.h>

namespace uvpp {
static void uvCloseDelete(void* handle) {
  uv_close((uv_handle_t*)handle, [](uv_handle_t* handleInner) { delete (uv_timer_t*)handleInner; });
}

using TimerDataHandleSPtr = std::shared_ptr<IntervalHandle>;

void intervalCb(uv_timer_t* uvHandle) { (*(TimerDataHandleSPtr*)uvHandle->data)->callback(); }

void IntervalHandle::cancel() {
  if (this->handle == nullptr) return;

  uv_timer_t* uvHandle = (uv_timer_t*)this->handle;

  TimerDataHandleSPtr* intervalHandleInner = (TimerDataHandleSPtr*)uvHandle->data;
  delete intervalHandleInner;

  uv_timer_stop(uvHandle);
  uvCloseDelete(uvHandle);

  this->handle = nullptr;
}

bool IntervalHandle::isRunning() {
  if (this->handle == nullptr) return false;

  uv_timer_t* uvHandle = (uv_timer_t*)this->handle;
  return uv_is_active((uv_handle_t*)uvHandle);
}

std::shared_ptr<IntervalHandle> uvIntervalStart(uint64_t intervalMs, uint64_t delayMs, IntervalCallback cb,
                                                uv_loop_t* loop) {
  if (loop == nullptr) loop = uv_default_loop();

  uv_timer_t* uvHandle = new uv_timer_t();

  std::shared_ptr<IntervalHandle> intervalHandle = std::make_shared<IntervalHandle>(uvHandle, std::move(cb));

  uvHandle->data = new TimerDataHandleSPtr(intervalHandle);

  uv_timer_init(loop, uvHandle);
  uv_timer_start(uvHandle, intervalCb, delayMs, intervalMs);

  return intervalHandle;
}
}  // namespace uvpp
