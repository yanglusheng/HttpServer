cmake_minimum_required(VERSION 3.26)
project(HttpServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(HttpServer main.cpp
        http_conn.cpp
        http_conn.h
        locker.h
        threadpool.h
)
