asynccpp
=====

C++20 event-loop agnostic coroutines (`co_await`/`co_return`) implementation + support for libuv and curl.

## Modules

### asyncpp

Event loop-agnostic implementation of C++20 coroutines (`co_await`/`co_return`).

Provides `makeTask` function which takes a callback with `resolve` and `reject` passed as arguments. Similar to how JavaScript `new Promise` constructor works.

### uvpp

libuv library wrappers for usage with C++ std::function.

### uv_tcptls

libuv OpenSSL TLS implementation.

### asyncpp_uv

libuv library wrappers to be used in coroutine-based program.

Implemented functions:

* `uvSleep` - waits passed amount of time before continuing.

### asyncpp_uv_curl

libuv based, coroutine API for curl.

## Example

```c++
#include <asyncpp_uv/asyncpp_uv.h>
#include <uv.h>

asyncpp::task<void> asyncMain() {
  printf("asyncMain BEGIN\n");
  auto timer1 = asyncpp_uv::uvSleepAsync(1000);
  auto timer2 = asyncpp_uv::uvSleepAsync(1000);
  co_await timer1;
  co_await timer2;
  printf("asyncMain END\n");
}

int main() {
  asyncMain();
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
```