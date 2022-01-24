add_library(uvpp STATIC
        ${CMAKE_CURRENT_LIST_DIR}/src/uvpp_async.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/uvpp_timer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/uvpp_interval.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/uvpp_close.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/uvpp_stream.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/uvpp_tcp.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/uvpp_dns.cpp

        # Header only
        ${CMAKE_CURRENT_LIST_DIR}/include/uvpp/uvpp_channel.h
        )
target_link_libraries(uvpp PUBLIC -luv)
target_include_directories(uvpp PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
set_property(TARGET uvpp PROPERTY CXX_STANDARD 20)