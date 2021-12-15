set(CMAKE_CXX_STANDARD 20)

add_executable(example_readme example_readme.cpp)
target_link_libraries(example_readme asyncpp_uv)

add_executable(example1_sleep example1_sleep.cpp)
target_link_libraries(example1_sleep asyncpp_uv)

add_executable(example2_curl example2_curl.cpp)
target_link_libraries(example2_curl asyncpp_uv_curl)
