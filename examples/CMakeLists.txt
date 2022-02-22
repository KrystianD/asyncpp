set(CMAKE_CXX_STANDARD 20)

add_executable(example_readme example_readme.cpp)
target_link_libraries(example_readme asyncpp_uv)

add_executable(example_sleep example_sleep.cpp)
target_link_libraries(example_sleep asyncpp_uv)

add_executable(example_timer example_timer.cpp)
target_link_libraries(example_timer asyncpp_uv)

add_executable(example_interval example_interval.cpp)
target_link_libraries(example_interval asyncpp_uv)

add_executable(example_then example_then.cpp)
target_link_libraries(example_then asyncpp_uv)

add_executable(example_curl example_curl.cpp)
target_link_libraries(example_curl asyncpp_uv_curl)

add_executable(example_curl_sync example_curl_sync.cpp)
target_link_libraries(example_curl_sync asyncpp_uv_curl)

add_executable(example_exception example_exception.cpp)
target_link_libraries(example_exception asyncpp_uv)

add_executable(example_queue example_queue.cpp)
target_link_libraries(example_queue asyncpp_uv)

add_executable(example_tcptls example_tcptls.cpp)
target_link_libraries(example_tcptls uv_tcptls)

add_executable(example_async example_async.cpp)
target_link_libraries(example_async asyncpp asyncpp_uv uvpp pthread)

add_executable(example_executor example_executor.cpp)
target_link_libraries(example_executor asyncpp asyncpp_uv uvpp pthread)

add_executable(example_channel example_channel.cpp)
target_link_libraries(example_channel asyncpp asyncpp_uv uvpp pthread)

add_executable(example_cancel example_cancel.cpp)
target_link_libraries(example_cancel asyncpp asyncpp_uv)
