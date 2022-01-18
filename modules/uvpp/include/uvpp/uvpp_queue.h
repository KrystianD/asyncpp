#pragma once

#include <uv.h>

#include <cstdint>
#include <functional>
#include <memory>

namespace uvpp {
template<typename T>
using WorkCallback = std::function<T()>;
template<typename T>
using AfterWorkCallback = std::function<void(T)>;

template<typename T>
void uvRunInThread(WorkCallback<T> workCb, AfterWorkCallback<T> afterWorkCb, uv_loop_t* loop = nullptr) {
  struct Data {
    WorkCallback<T> workCb;
    AfterWorkCallback<T> afterWorkCb;
    T result;
  };

  Data* data = new Data();
  data->workCb = std::move(workCb);
  data->afterWorkCb = std::move(afterWorkCb);

  uv_work_t* handle = new uv_work_t();
  handle->data = data;

  uv_queue_work(
      loop ? loop : uv_default_loop(), handle,
      [](uv_work_t* handleInner) {
        Data* data = (Data*)handleInner->data;
        data->result = std::move(data->workCb());
      },
      [](uv_work_t* handleInner, int status) {
        Data* data = (Data*)handleInner->data;

        data->afterWorkCb(std::move(data->result));

        delete data;
        delete handleInner;
      });
}
}  // namespace uvpp
