cmake_minimum_required(VERSION 3.6)
project(clustering)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lm")

set(SOURCE_FILES src/main.c)

add_executable(clustering ${SOURCE_FILES})

target_link_libraries(clustering m)  # links math library to project