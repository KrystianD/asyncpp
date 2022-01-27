#pragma once

#include <functional>
#include <memory>
#include <vector>

namespace asyncpp {
class cancellation_token;
class cancellation_token_source;

class cancellation_token {
  struct _state;

  std::shared_ptr<_state> _impl;

  cancellation_token();

  void cancel() const;

 public:
  void on_cancel(std::function<void()> cancel_cb);

  static const cancellation_token empty;

  friend class cancellation_token_source;
};

class cancellation_token_source {
  cancellation_token _token;

 public:
  cancellation_token get_token() { return _token; }

  void cancel() const { _token.cancel(); }
};
}  // namespace asyncpp
