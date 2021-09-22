cmake_minimum_required(VERSION 3.20)
project(HelloServ)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS -pthread)

add_executable(HelloServ src/main.cpp src/util/util.h src/util/util.cpp src/util/csapp.h src/util/csapp.cpp src/echo.h)
