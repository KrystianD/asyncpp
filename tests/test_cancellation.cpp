#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv.h>

#include "test.h"

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

TEST(CancellationTest, CancelAsync) {
  EXPECT_THROW(runAsync([]() -> task<void> {
                 cancellation_token_source tcs;

                 uvpp::uvTimerStart(1s, [tcs]() { tcs.cancel(); });

                 co_await uvSleepAsync(10s, tcs.get_token());
               }),
               task_cancelled);
}

TEST(CancellationTest, CancelAsyncMultiple) {
  EXPECT_THROW(runAsync([]() -> task<void> {
                 cancellation_token_source tcs;

                 uvpp::uvTimerStart(1s, [tcs]() { tcs.cancel(); });

                 auto s1 = uvSleepAsync(10s, tcs.get_token());
                 auto s2 = uvSleepAsync(5s, tcs.get_token());

                 co_await s1;
                 co_await s2;
               }),
               task_cancelled);
}

TEST(CancellationTest, CancelWithoutReference) {
  EXPECT_NO_THROW(runAsync([]() -> task<void> {
    cancellation_token_source tcs;

    uvpp::uvTimerStart(1s, [tcs]() { tcs.cancel(); });

    uvSleepAsync(10s, tcs.get_token());

    co_return;
  }));
}

TEST(CancellationTest, CancelWithoutReferenceCapture) {
  bool captured = false;

  EXPECT_NO_THROW(runAsync([&captured]() -> task<void> {
    cancellation_token_source tcs;

    uvpp::uvTimerStart(500ms, [tcs]() { tcs.cancel(); });

    uvSleepAsync(10s, tcs.get_token())
        .then([]() {},
              [&captured](auto excPtr) {
                try {
                  std::rethrow_exception(excPtr);
                } catch (task_cancelled&) {
                  captured = true;
                }
              });

    co_await uvSleepAsync(1s);
  }));

  EXPECT_TRUE(captured);
}

TEST(CancellationTest, DoubleCancel) {
  bool captured = false;

  EXPECT_NO_THROW(runAsync([&captured]() -> task<void> {
    cancellation_token_source tcs;

    uvpp::uvTimerStart(500ms, [tcs]() { tcs.cancel(); });
    uvpp::uvTimerStart(600ms, [tcs]() { tcs.cancel(); });

    uvSleepAsync(10s, tcs.get_token())
        .then([]() {},
              [&captured](auto excPtr) {
                try {
                  std::rethrow_exception(excPtr);
                } catch (task_cancelled&) {
                  captured = true;
                }
              });

    co_await uvSleepAsync(1s);
  }));

  EXPECT_TRUE(captured);
}
