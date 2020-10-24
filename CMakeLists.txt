project(pchat)
cmake_minimum_required(VERSION 3.2)

SET(LIBEVENT_LIB "event" CACHE STRING "Libevent library name for linking")

set(CMAKE_CFLAGS "${CMAKE_CFLAGS} -Wall -Wextra -Werror")

file(GLOB SOURCES "*.c")

add_executable(pchat ${SOURCES})
target_link_libraries(pchat ${LIBEVENT_LIB})

