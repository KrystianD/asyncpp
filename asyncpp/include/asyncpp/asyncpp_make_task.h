#pragma once

#include <functional>

namespace asyncpp {
template<typename T>
using ResolveResultCb = std::function<void(T&&)>;

using ResolveVoidCb = std::function<void()>;

template<typename T, typename TState>
[[maybe_unused]] task<T> makeTaskWithState(const std::function<void(TState&, ResolveResultCb<T>&)>& cb) {
  auto state = std::make_shared<awaitable_state<T>>();

  std::shared_ptr<TState> customState = std::make_shared<TState>();

  ResolveResultCb<T> resolve = [state](const T& value) { state->set_value(value); };

  cb(*customState, resolve);

  return task<T>(state, customState);
}

template<typename TState>
[[maybe_unused]] task<void> makeTaskWithState(const std::function<void(TState&, ResolveVoidCb&)>& cb) {
  auto state = std::make_shared<awaitable_state<void>>();

  std::shared_ptr<TState> customState = std::make_shared<TState>();

  ResolveVoidCb resolve = [state]() { state->set_value(); };

  cb(*customState, resolve);

  return task<void>(state, customState);
}

template<typename T>
[[maybe_unused]] task<T> makeTask(const std::function<void(ResolveResultCb<T>&)>& cb) {
  auto state = std::make_shared<awaitable_state<T>>();

  ResolveResultCb<T> resolve = [state](T&& value) { state->set_value(std::move(value)); };

  cb(resolve);

  return task<T>(state);
}

[[maybe_unused]] static task<void> makeTask(const std::function<void(ResolveVoidCb&)>& cb) {
  auto state = std::make_shared<awaitable_state<void>>();

  ResolveVoidCb resolve = [state]() { state->set_value(); };

  cb(resolve);

  return task<void>(state);
}
}  // namespace asyncpp
