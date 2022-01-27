#include <asyncpp/asyncpp.h>

namespace asyncpp {

struct cancellation_token::_state {
  std::vector<std::function<void()>> _cancel_cbs;
};

cancellation_token::cancellation_token() : _impl(std::shared_ptr<_state>(new cancellation_token::_state())) {}

void cancellation_token::cancel() const {
  for (const auto& item : _impl->_cancel_cbs) {
    item();
  }
}

void cancellation_token::on_cancel(std::function<void()> cancel_cb) {
  _impl->_cancel_cbs.emplace_back(std::move(cancel_cb));
}

const cancellation_token cancellation_token::empty;
}  // namespace asyncpp