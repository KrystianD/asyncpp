#pragma once

#include <functional>

#include "impl/cancellation.h"

namespace asyncpp {
template<typename T>
class Resolver {
  std::weak_ptr<awaitable_state<T>> _state;

 public:
  Resolver() = default;
  explicit Resolver(std::shared_ptr<awaitable_state<T>> state) : _state(std::move(state)) {}

  void operator()(T value) const {
    if (std::shared_ptr<awaitable_state<T>> state = _state.lock()) {
      state->set_value(std::move(value));
    }
  }
};

template<>
class Resolver<void> {
  std::weak_ptr<awaitable_state<void>> _state;

 public:
  Resolver() = default;
  explicit Resolver(std::shared_ptr<awaitable_state<void>> state) : _state(std::move(state)) {}

  void operator()() const {
    if (std::shared_ptr<awaitable_state<void>> state = _state.lock()) {
      state->set_value();
    }
  }
};

class Rejecter {
  std::weak_ptr<awaitable_state_base> _state;

 public:
  Rejecter() = default;
  explicit Rejecter(std::shared_ptr<awaitable_state_base> state) : _state(std::move(state)) {}

  template<typename E>
  void operator()(const E& value) const {
    operator()(std::make_exception_ptr(value));
  }

  void operator()(std::exception_ptr exc_ptr) const {
    if (std::shared_ptr<awaitable_state_base> state = _state.lock()) {
      state->set_exception(exc_ptr);
    } else {
      // nothing is holding a reference to this task, so exception can't be handled by the user, throwing
      // a general error

      fprintf(stderr, "Unhandled promise exception:\n");
      rethrow_exception(exc_ptr);
    }
  }
};

using TCancelCb = std::function<void()>;

template<typename T, typename TState>
[[maybe_unused]] task<T> makeTaskWithState(const std::function<void(TState&, Resolver<T>&, Rejecter&)>& cb) {
  auto state = std::make_shared<awaitable_state<T>>();

  std::shared_ptr<TState> customState = std::make_shared<TState>();

  Resolver<T> resolver(state);
  Rejecter rejecter(state);

  cb(*customState, resolver, rejecter);

  task<T> task(std::move(state));
  task.set_internal_data(std::move(customState));
  return task;
}

template<typename T, typename TState>
[[maybe_unused]] task<T> makeTaskWithState(const std::function<void(TState&, Resolver<T>&)>& cb) {
  auto state = std::make_shared<awaitable_state<T>>();

  std::shared_ptr<TState> customState = std::make_shared<TState>();

  Resolver<T> resolver(state);

  cb(*customState, resolver);

  task<T> task(std::move(state));
  task.set_internal_data(std::move(customState));
  return task;
}

template<typename TState>
[[maybe_unused]] task<void> makeTaskWithState(const std::function<void(TState&, Resolver<void>&, Rejecter&)>& cb) {
  auto state = std::make_shared<awaitable_state<void>>();

  std::shared_ptr<TState> customState = std::make_shared<TState>();

  Resolver<void> resolver(state);
  Rejecter rejecter(state);

  cb(*customState, resolver, rejecter);

  return task<void>(std::move(state), std::move(customState));
}

template<typename TState>
[[maybe_unused]] task<void> makeTaskWithState(const std::function<void(TState&, Resolver<void>&)>& cb) {
  auto state = std::make_shared<awaitable_state<void>>();

  std::shared_ptr<TState> customState = std::make_shared<TState>();

  Resolver<void> resolver(state);

  cb(*customState, resolver);

  return task<void>(std::move(state), std::move(customState));
}

template<typename T>
[[maybe_unused]] task<T> makeTask(const std::function<void(Resolver<T>&, Rejecter&)>& cb) {
  auto state = std::make_shared<awaitable_state<T>>();

  Resolver<T> resolver(state);
  Rejecter rejecter(state);

  cb(resolver, rejecter);

  return task<T>(std::move(state));
}

template<typename T>
[[maybe_unused]] task<T> makeTask(const std::function<void(Resolver<T>&)>& cb) {
  auto state = std::make_shared<awaitable_state<T>>();

  Resolver<T> resolver(state);

  cb(resolver);

  return task<T>(std::move(state));
}

[[maybe_unused]] static task<void> makeTask(const std::function<void(Resolver<void>&, Rejecter&)>& cb) {
  auto state = std::make_shared<awaitable_state<void>>();

  Resolver<void> resolver(state);
  Rejecter rejecter(state);

  cb(resolver, rejecter);

  return task<void>(std::move(state));
}

[[maybe_unused]] static task<void> makeTask(const std::function<void(Resolver<void>&)>& cb) {
  auto state = std::make_shared<awaitable_state<void>>();

  Resolver<void> resolver(state);

  cb(resolver);

  return task<void>(std::move(state));
}

template<typename T>
[[maybe_unused]] static cancellable_task<T> makeCancellableTask(const std::function<TCancelCb(Resolver<T>&)>& cb,
                                                                cancellation_token token = cancellation_token::empty) {
  auto state = std::make_shared<awaitable_state<T>>();

  Resolver<T> resolver(state);

  TCancelCb cancel_cb = cb(resolver);
  state->on_cancel(std::move(cancel_cb));

  cancellable_task<T> task(std::move(state));
  task.set_cancellation_token(token);
  return task;
}

[[maybe_unused]] static cancellable_task<void> makeCancellableTask(
    const std::function<TCancelCb(Resolver<void>&)>& cb, cancellation_token token = cancellation_token::empty) {
  auto state = std::make_shared<awaitable_state<void>>();

  Resolver<void> resolver(state);

  TCancelCb cancel_cb = cb(resolver);
  state->on_cancel(std::move(cancel_cb));

  cancellable_task<void> task(std::move(state));
  task.set_cancellation_token(token);
  return task;
}

template<typename T>
[[maybe_unused]] static cancellable_task<T> makeCancellableTask(
    const std::function<TCancelCb(Resolver<T>&, Rejecter&)>& cb, cancellation_token token = cancellation_token::empty) {
  auto state = std::make_shared<awaitable_state<T>>();

  Resolver<T> resolver(state);
  Rejecter rejecter(state);

  TCancelCb cancel_cb = cb(resolver, rejecter);
  state->on_cancel(std::move(cancel_cb));

  cancellable_task<T> task(std::move(state));
  task.set_cancellation_token(token);
  return task;
}

[[maybe_unused]] static cancellable_task<void> makeCancellableTask(
    const std::function<TCancelCb(Resolver<void>&, Rejecter&)>& cb,
    cancellation_token token = cancellation_token::empty) {
  auto state = std::make_shared<awaitable_state<void>>();

  Resolver<void> resolver(state);
  Rejecter rejecter(state);

  TCancelCb cancel_cb = cb(resolver, rejecter);
  state->on_cancel(std::move(cancel_cb));

  cancellable_task<void> task(std::move(state));
  task.set_cancellation_token(token);
  return task;
}
}  // namespace asyncpp
