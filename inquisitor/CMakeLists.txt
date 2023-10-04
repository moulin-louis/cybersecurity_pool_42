cmake_minimum_required(VERSION 3.26)
project(inquisitor C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -g)

include_directories(inc)

add_executable(inquisitor
        inc/inquisitor.h
        src/arp.c
        src/main.c
        src/utils.c)
