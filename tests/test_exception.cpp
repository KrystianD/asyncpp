#include <asyncpp/asyncpp.h>
#include <asyncpp_uv/asyncpp_uv.h>

#include "test.h"

using namespace std;
using namespace asyncpp;
using namespace asyncpp_uv;

TEST(ExceptionsTest, ThrowNoAsync) {
  EXPECT_THROW(runAsync([]() -> task<void> { throw std::runtime_error("error5"); }), std::runtime_error);
}

TEST(ExceptionsTest, Rethrow) {
  EXPECT_THROW(runAsync([]() -> task<void> {
                 auto f = []() -> task<void> { throw std::runtime_error("error5"); };
                 try {
                   co_await f();
                 } catch (std::exception &exc) {
                   printf("exception exc\n");
                   throw;
                 }
               }),
               std::runtime_error);
}

TEST(ExceptionsTest, ThrowEarly) {
  EXPECT_THROW(runAsync([]() -> task<void> {
                 throw std::runtime_error("error1");
                 co_await uvSleepAsync(100);
               }),
               std::runtime_error);
}

TEST(ExceptionsTest, ThrowLate) {
  EXPECT_THROW(runAsync([]() -> task<void> {
                 co_await uvSleepAsync(100);
                 throw std::runtime_error("error2");
               }),
               std::runtime_error);
}

TEST(ExceptionsTest, ThrowViaReject) {
  EXPECT_THROW(runAsync([]() -> task<void> {
                 return asyncpp::makeTask([](auto resolve, auto reject) { reject(std::runtime_error("error4")); });
               }),
               std::runtime_error);
}
