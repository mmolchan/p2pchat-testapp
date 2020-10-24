project(pchat)
cmake_minimum_required(VERSION 3.2)

set(LIBEVENT_LIB "event" CACHE STRING "Libevent library name for linking")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

file(GLOB SOURCES "*.c")

add_executable(pchat ${SOURCES})
target_link_libraries(pchat ${LIBEVENT_LIB})

