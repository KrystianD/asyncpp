add_library(asyncpp_uv STATIC
        ${CMAKE_CURRENT_LIST_DIR}/src/asyncpp_uv_timer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/asyncpp_uv_sleep.cpp
        )
target_link_libraries(asyncpp_uv PUBLIC asyncpp -luv)
target_include_directories(asyncpp_uv PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
set_property(TARGET asyncpp_uv PROPERTY CXX_STANDARD 20)