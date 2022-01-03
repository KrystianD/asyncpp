#include "test.h"

int main(int argc, char *argv[]) {
  bool stop = false;
  std::thread th([&stop]() {
    while (!stop) {
      int res;
      res = uv_run(uv_default_loop(), UV_RUN_ONCE);
      if (res == 0) uv_sleep(100);
    }
  });
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  stop = true;

  th.join();

  return result;
}
