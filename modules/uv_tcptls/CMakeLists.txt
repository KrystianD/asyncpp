add_library(uv_tcptls STATIC
        ${CMAKE_CURRENT_LIST_DIR}/src/uv_tcptls.c
        )
target_link_libraries(uv_tcptls PUBLIC uvpp -lssl -lcrypto)
target_include_directories(uv_tcptls PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
set_property(TARGET uv_tcptls PROPERTY CXX_STANDARD 20)