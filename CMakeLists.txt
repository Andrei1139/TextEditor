cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(TextEditor)

add_definitions(-DUNICODE -D_UNICODE)

file(GLOB SRCS src/*.cpp)

add_executable(TextEditor ${SRCS})