#include "test.h"

int main(int argc, char *argv[]) {
  bool stop = false;
  std::thread th([&stop]() {
    int res;
    do {
      res = uv_run(uv_default_loop(), UV_RUN_ONCE);
      if (res == 0) uv_sleep(100);
    } while (!stop || res != 0);
  });
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  stop = true;

  th.join();

  return result;
}
