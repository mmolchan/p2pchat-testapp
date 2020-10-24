project(pchat)
cmake_minimum_required(VERSION 3.2)

set(LIBEVENT_LIB "event" CACHE STRING "Libevent library name for linking")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

file(GLOB SOURCES "*.c")
file(GLOB SOURCES_3RD "3rdparty/*.c")

add_executable(pchat ${SOURCES} ${SOURCES_3RD})
target_link_libraries(pchat ${LIBEVENT_LIB})

