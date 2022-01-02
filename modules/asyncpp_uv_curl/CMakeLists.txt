add_library(asyncpp_uv_curl STATIC src/curl_uv.cpp)
target_link_libraries(asyncpp_uv_curl PUBLIC asyncpp_uv -lcrypto -lssl -lcurl)
target_include_directories(asyncpp_uv_curl PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
set_property(TARGET asyncpp_uv_curl PROPERTY CXX_STANDARD 20)