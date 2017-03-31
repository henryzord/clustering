cmake_minimum_required(VERSION 3.6)
project(clustering)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES src/main.c src/measures/sswc.h)

link_directories(src/measures)

add_executable(clustering ${SOURCE_FILES})