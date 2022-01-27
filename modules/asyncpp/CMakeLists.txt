add_library(asyncpp STATIC
        ${CMAKE_CURRENT_LIST_DIR}/src/asyncpp.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/cancellation.cpp
        )
target_include_directories(asyncpp PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_options(asyncpp PUBLIC -fcoroutines)
set_property(TARGET asyncpp PROPERTY CXX_STANDARD 20)