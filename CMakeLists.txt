cmake_minimum_required(VERSION 3.24)
project(fat16parser C)

set(CMAKE_C_STANDARD 99)

add_executable(fat16parser main.c disk_parser.c disk_parser.h)
