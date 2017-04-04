cmake_minimum_required(VERSION 3.6)
project(clustering)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lm -std=c11")

set(SOURCE_FILES src/main.c src/utils.h src/measures.h)

add_executable(clustering ${SOURCE_FILES})

target_link_libraries(clustering m)  # links math library to project