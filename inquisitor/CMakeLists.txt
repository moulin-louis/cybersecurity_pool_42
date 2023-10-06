cmake_minimum_required(VERSION 3.24)
project(inquisitor C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -Werror -g)

include_directories(inc)

add_executable(
        inquisitor
        src/arp.c
        src/main.c
        src/utils.c
        src/packet.c)
