cmake_minimum_required(VERSION 2.6)
project(iterator_based_for)

add_executable(iterator_based_for main.cpp)

install(TARGETS iterator_based_for RUNTIME DESTINATION bin)
