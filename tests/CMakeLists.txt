set(CMAKE_CXX_STANDARD 20)

add_executable(test_then test_then.cpp)
target_link_libraries(test_then asyncpp_uv)

add_executable(test test_main.cpp
        test_exception.cpp
        test_ref.cpp
        test_cancellation.cpp
        )
target_link_libraries(test asyncpp_uv gtest pthread)
