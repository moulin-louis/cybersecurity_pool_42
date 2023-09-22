cmake_minimum_required(VERSION 3.13)
project(iron_dome)

set(CMAKE_CXX_STANDARD 17)
include_directories(inc)
add_executable(iron_dome src/main.cpp
        src/IronDome.cpp
        inc/IronDome.hpp
)
