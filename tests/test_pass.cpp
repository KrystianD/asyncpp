#include <asyncpp/asyncpp.h>

#include "obj.h"
#include "test.h"

using namespace std;
using namespace asyncpp;

using obj::Obj;

task<double> get_int() { co_return 1; }
task<double> get_double() { co_return 1.2; }
task<Obj> get_obj() { co_return Obj(2); }

TEST(PassTest, GetInt) {
  runAsync([]() -> task<void> { co_await get_int(); });
}

TEST(PassTest, GetDouble) {
  runAsync([]() -> task<void> { co_await get_double(); });
}

TEST(PassTest, GetObj) {
  obj::clear();
  Obj tmp;

  runAsync([&tmp]() -> task<void> { tmp = co_await get_obj(); });

  ASSERT_EQ(2, tmp.v);
  ASSERT_EQ(0, obj::copies);
  ASSERT_EQ(3, obj::moves);
}
