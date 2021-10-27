cmake_minimum_required(VERSION 3.10)
project(http_proxy_server)

set(CMAKE_CXX_STANDARD 14)

add_executable(http_proxy_server
        src/main.cpp
        src/ProxyServer.cpp src/ProxyServer.h
        src/ServerConfig.cpp src/ServerConfig.h
        src/Proxy.cpp src/Proxy.h
        src/logging.h
        src/utils/str_utl.cpp src/utils/str_utl.h
        src/http_message/HTTPMessage.cpp src/http_message/HTTPMessage.h
        src/http_message/Request.cpp src/http_message/Request.h
        src/http_message/Response.cpp src/http_message/Response.h
        src/utils/http_utl.cpp src/utils/http_utl.h
        src/ThreadPool.cpp src/ThreadPool.h
        src/utils/socket_utl.cpp src/utils/socket_utl.h src/set_signals.cpp src/set_signals.h src/BlacklistProxy.cpp src/BlacklistProxy.h)

find_package(Threads)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
