cmake_minimum_required(VERSION 3.24)

project(inquisitor)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-g)

include_directories(inc)

add_executable(
        inquisitor
        src/arp.cpp
        src/main.cpp
        src/utils.cpp
        src/packet.cpp)

target_link_libraries(inquisitor pcap)