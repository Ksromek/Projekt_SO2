cmake_minimum_required(VERSION 3.28.6)
project(Projekt)

set(CMAKE_CXX_STANDARD 20)
include_directories(include)

add_executable(Projekt1 "Projekt 1/main1.cpp")
add_executable(Serwer "Projekt 2/server.cpp")
add_executable(Klient "Projekt 2/client.cpp")

target_link_libraries(Serwer ws2_32)
target_link_libraries(Klient ws2_32)