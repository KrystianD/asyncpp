#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv.h>
#include <uvpp/uvpp.h>

#include "test.h"

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;
using namespace uvpp;

bool allEqual;

TEST(RefTest, Ref) {
  allEqual = true;

  EXPECT_NO_THROW(runAsync([]() -> task<void> {
    bool value = false;

    asyncpp::run([&value]() -> task<void> {
      bool &local = value;

      for (int i = 0; i < 10; i++) {
        co_await uvSleepAsync(50);
        if (&local != &value) allEqual = false;
      }
    });

    co_await uvSleepAsync(100);
  }));

  EXPECT_TRUE(allEqual);
}

/*
 * Verify that invalid way of spawning the coroutine causes losing std::function closure.
 * Test is disabled as it results in Invalid Read/Write in valgrind.
 */
TEST(RefTest, DISABLED_ExpectRefIssue) {
  allEqual = true;

  EXPECT_NO_THROW(runAsync([]() -> task<void> {
    bool value = false;

    // Invalid way of spawning a temporary coroutine!
    [&value]() -> task<void> {
      bool &local = value;

      for (int i = 0; i < 10; i++) {
        co_await uvSleepAsync(50);
        if (&local != &value) allEqual = false;
      }
    }();

    co_await uvSleepAsync(100);
  }));

  EXPECT_FALSE(allEqual);
}
