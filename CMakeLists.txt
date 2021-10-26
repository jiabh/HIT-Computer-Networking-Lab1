cmake_minimum_required(VERSION 3.10)
project(http_proxy_server)

set(CMAKE_CXX_STANDARD 14)

add_executable(http_proxy_server
        src/main.cpp
        src/ProxyServer.cpp src/ProxyServer.h
        src/ServerConfig.cpp src/ServerConfig.h
        src/Proxy.cpp src/Proxy.h
        src/logging.h
        src/util.cpp src/util.h
        src/http_message/HTTPMessage.cpp src/http_message/HTTPMessage.h
        src/http_message/Request.cpp src/http_message/Request.h
        src/http_message/Response.cpp src/http_message/Response.h
        src/http_message/util.cpp src/http_message/util.h
        )
