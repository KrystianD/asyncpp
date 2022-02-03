/*
 * Based on https://github.com/jimspr/awaituv
 */

#pragma once

#include <cassert>
#include <coroutine>
#include <functional>
#include <memory>

#include "impl/cancellation.h"

namespace asyncpp {
enum struct future_error {
  not_ready,  // get_value called when value not available
};

struct future_exception : std::exception {
  future_error _error;
  explicit future_exception(future_error fe) : _error(fe) {}
};

struct task_timeout : std::exception {
  const char* what() const noexcept override { return "task timeout"; }
};

struct task_cancelled : std::exception {
  const char* what() const noexcept override { return "task cancelled"; }
};

struct awaitable_state_base {
  std::function<void(void)> _coro;
  std::function<void(void)> _cancel_cb;
  bool _ready = false;
  std::exception_ptr _exception;
  std::shared_ptr<void> _custom_data;

  awaitable_state_base() = default;
  awaitable_state_base(awaitable_state_base&&) = delete;
  awaitable_state_base(const awaitable_state_base&) = delete;

  ~awaitable_state_base() {
    if (_exception) {
      // throw the saved exception if not awaited (ignore task_cancelled)
      try {
        rethrow_exception(_exception);
      } catch (task_cancelled&) {
      } catch (std::exception&) {
        fprintf(stderr, "Unhandled promise exception:\n");
        rethrow_exception(_exception);
      }
    }
  }

  void set_coroutine_callback(std::function<void(void)> cb) {
    // Test to make sure nothing else is waiting on this future.
    assert(((cb == nullptr) || (_coro == nullptr)) && "This future is already being awaited.");
    _coro = std::move(cb);
  }

  void continue_coroutine() {
    // Set all members first as calling coroutine may reset stuff here.
    _ready = true;
    auto coro = std::move(_coro);
    _coro = nullptr;
    if (coro != nullptr) coro();
  }

  void set_value() { continue_coroutine(); }

  void set_exception(const std::exception_ptr& exception_ptr) {
    _exception = exception_ptr;
    continue_coroutine();
  }

  [[nodiscard]] bool ready() const { return _ready; }

  void set_data(std::shared_ptr<void> data) { _custom_data = data; }

  void on_cancel(std::function<void(void)> cancel_cb) { _cancel_cb = std::move(cancel_cb); }

  void cancel() {
    if (_ready) {
      return;
    }
    if (_cancel_cb) {
      _cancel_cb();
    }
    _exception = std::make_exception_ptr(task_cancelled());
    continue_coroutine();
  }
};

template<typename TValue>
struct awaitable_state : public awaitable_state_base {
  TValue _value;

  void set_value(TValue&& t) {
    _value = std::move(t);
    awaitable_state_base::set_value();
  }

  TValue&& get_value() {
    if (!_ready) throw future_exception(future_error::not_ready);
    return std::move(_value);
  }
};

// specialization of awaitable_state<void>
template<>
struct awaitable_state<void> : public awaitable_state_base {
  void get_value() const {
    if (!_ready) throw future_exception(future_error::not_ready);
  }
};

template<typename T>
struct promise_t;

template<typename T>
class task {
 protected:
  std::shared_ptr<awaitable_state<T>> _state;

 public:
  // promise_type declaration required by C++20 coroutines
  typedef promise_t<T> promise_type;

  task() = default;

  explicit task(std::shared_ptr<awaitable_state<T>> state) : _state(std::move(state)) {}

  // not copyable
  task(const task&) = delete;
  task& operator=(const task&) = delete;
  // movable
  task(task&&) noexcept = default;
  task& operator=(task&&) noexcept = default;

  T await_resume() const {
    if (_state->_exception) {
      std::rethrow_exception(std::move(_state->_exception));
    }
    return std::move(_state->get_value());
  }

  [[nodiscard]] bool await_ready() const { return _state->_ready; }

  void await_suspend(std::coroutine_handle<> resume_cb) { _state->set_coroutine_callback(resume_cb); }

  void then(std::function<void(T)> resume_cb) {
    auto state = _state;

    if (await_ready()) {
      if (!state->_exception) {
        resume_cb(std::move(state->get_value()));
      }
    } else {
      state->set_coroutine_callback([state = std::move(state), resume_cb = std::move(resume_cb)]() {
        if (!state->_exception) {
          resume_cb(std::move(state->get_value()));
        }
      });
    }
  }

  void then(std::function<void(T)> resume_cb, std::function<void(std::exception_ptr)> error_cb) {
    auto state = _state;

    if (await_ready()) {
      if (!state->_exception) {
        resume_cb(std::move(state->get_value()));
      } else {
        error_cb(std::move(state->_exception));
      }
    } else {
      state->set_coroutine_callback([state, resume_cb = std::move(resume_cb), error_cb = std::move(error_cb)]() {
        if (!state->_exception) {
          resume_cb(std::move(state->get_value()));
        } else {
          error_cb(std::move(state->_exception));
        }
      });
    }
  }

  void disconnect() { _state->set_coroutine_callback(nullptr); }

  void set_internal_data(std::shared_ptr<void> data) { _state->set_data(data); }
};

template<>
class task<void> {
 protected:
  std::shared_ptr<awaitable_state<void>> _state;

 public:
  // promise_type declaration required by C++20 coroutines
  typedef promise_t<void> promise_type;

  task() = default;

  explicit task(std::shared_ptr<awaitable_state<void>> state) : _state(std::move(state)) {}

  // not copyable
  task(const task&) = delete;
  task& operator=(const task&) = delete;
  // movable
  task(task&&) noexcept = default;
  task& operator=(task&&) noexcept = default;

  void await_resume() const {
    if (_state->_exception) {
      std::rethrow_exception(std::move(_state->_exception));
    }
  }

  [[nodiscard]] bool await_ready() const { return _state->_ready; }

  void await_suspend(std::coroutine_handle<> resume_cb) { _state->set_coroutine_callback(resume_cb); }

  void then(std::function<void()> resume_cb) {
    auto state = _state;

    if (await_ready()) {
      if (!state->_exception) {
        resume_cb();
      }
    } else {
      state->set_coroutine_callback([state, resume_cb = std::move(resume_cb)]() {
        if (!state->_exception) {
          resume_cb();
        }
      });
    }
  }

  void then(std::function<void()> resume_cb, std::function<void(std::exception_ptr)> error_cb) {
    auto state = _state;

    if (await_ready()) {
      if (!state->_exception) {
        resume_cb();
      } else {
        error_cb(std::move(state->_exception));
      }
    } else {
      state->set_coroutine_callback([state, resume_cb = std::move(resume_cb), error_cb = std::move(error_cb)]() {
        if (!state->_exception) {
          resume_cb();
        } else {
          error_cb(std::move(state->_exception));
        }
      });
    }
  }

  void disconnect() { _state->set_coroutine_callback(nullptr); }

  void set_internal_data(std::shared_ptr<void> data) { _state->set_data(data); }
};

template<typename T>
class cancellable_task : public task<T> {
 public:
  explicit cancellable_task(std::shared_ptr<awaitable_state<T>> state) : task<T>(std::move(state)) {}

  void set_cancellation_token(cancellation_token token) {
    token.on_cancel([state = this->_state]() { state->cancel(); });
  }
};

template<>
class cancellable_task<void> : public task<void> {
 public:
  explicit cancellable_task(std::shared_ptr<awaitable_state<void>> state) : task<void>(std::move(state)) {}

  void set_cancellation_token(cancellation_token token) {
    token.on_cancel([state = _state]() { state->cancel(); });
  }
};

template<typename T>
struct promise_t {
  std::shared_ptr<awaitable_state<T>> state;

  promise_t() : state(std::make_shared<awaitable_state<T>>()) {}

  // not copyable
  promise_t(const promise_t&) = delete;
  promise_t& operator=(const promise_t&) = delete;
  // movable
  promise_t(promise_t&&) noexcept = default;
  promise_t& operator=(promise_t&&) noexcept = default;

  task<T> get_return_object() { return task(state); }

  [[nodiscard]] std::suspend_never initial_suspend() const noexcept { return {}; }
  [[nodiscard]] std::suspend_never final_suspend() const noexcept { return {}; }

  void return_value(T val) { state->set_value(std::move(val)); }

  void unhandled_exception() { state->set_exception(std::current_exception()); }
};

template<>
struct promise_t<void> {
  std::shared_ptr<awaitable_state<void>> state;

  explicit promise_t() : state(std::make_shared<awaitable_state<void>>()) {}

  // not copyable
  promise_t(const promise_t&) = delete;
  promise_t& operator=(const promise_t&) = delete;
  // movable
  promise_t(promise_t&&) = default;
  promise_t& operator=(promise_t&&) = default;

  task<void> get_return_object() { return task(state); }

  [[nodiscard]] std::suspend_never initial_suspend() const noexcept { return {}; }
  [[nodiscard]] std::suspend_never final_suspend() const noexcept { return {}; }

  void return_void() { state->continue_coroutine(); }

  void unhandled_exception() { state->set_exception(std::current_exception()); }
};

template<typename T>
[[maybe_unused]] task<T> run(std::function<task<T>()> fn) {
  struct state {
    std::function<task<T>()> fn;
  };

  auto statePtr = std::make_shared<state>();
  statePtr->fn = std::move(fn);

  task<T> task = statePtr->fn();
  task.set_internal_data(statePtr);
  return task;
}

[[maybe_unused]] static task<void> run(std::function<task<void>()> fn) {
  struct state {
    std::function<task<void>()> fn;
  };

  auto statePtr = std::make_shared<state>();
  statePtr->fn = std::move(fn);

  task<void> task = statePtr->fn();
  task.set_internal_data(statePtr);
  return task;
}
}  // namespace asyncpp
