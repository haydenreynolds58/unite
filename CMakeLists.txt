cmake_minimum_required(VERSION 3.16)
project(unite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(unite
    src/main.cpp
    src/player.cpp
)

target_include_directories(unite PRIVATE include)
target_link_libraries(unite PRIVATE raylib)