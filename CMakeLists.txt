cmake_minimum_required(VERSION 2.8.8)
project(queue_show)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(Pthread REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fno-strict-aliasing -std=gnu99")

message(STATUS "~~~~~~~~~~~~~~~~~~ BUILD TYPE '${CMAKE_BUILD_TYPE}' ~~~~~~~~~~~~~~~~~~")

set(LIBHL_DST_DIR "${CMAKE_BINARY_DIR}/libs/libhl")

include_directories("${LIBHL_DST_DIR}/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs")

# Сборка libhl
add_custom_target(
        libhl
        DEPENDS ${LIBHL_DST_DIR}/build/libhl.a
)
add_custom_command(
        OUTPUT ${LIBHL_DST_DIR}/build/configure
        COMMAND mkdir "${CMAKE_BINARY_DIR}/libs/" && tar -xzf "${CMAKE_CURRENT_SOURCE_DIR}/libs/libhl.tar.gz" --directory "${CMAKE_BINARY_DIR}/libs/"
)
add_custom_command(
        OUTPUT ${LIBHL_DST_DIR}/build/Makefile
        COMMAND cd ${LIBHL_DST_DIR}/build && ${LIBHL_DST_DIR}/build/configure -q > ${CMAKE_BINARY_DIR}/libhl.log 2>&1
        DEPENDS ${LIBHL_DST_DIR}/build/configure
)
add_custom_command(
        OUTPUT ${LIBHL_DST_DIR}/build/libhl.a
        COMMAND cd ${LIBHL_DST_DIR}/build && make clean > ${CMAKE_BINARY_DIR}/libhl.log 2>&1 && make > ${CMAKE_BINARY_DIR}/libhl.log 2>&1
        DEPENDS ${LIBHL_DST_DIR}/build/Makefile
)

add_library(QUEUE OBJECT
	tm_queue.c
	tm_queue_simple.c
	tm_queue_lockless.c
)

add_dependencies(QUEUE libhl)

set(SRC main.c tm_block.c tm_refcount.c)

add_executable(${CMAKE_PROJECT_NAME} ${SRC} $<TARGET_OBJECTS:QUEUE>)
target_link_libraries(${CMAKE_PROJECT_NAME}
	${LIBHL_DST_DIR}/build/libhl.a
	${PTHREAD_LIBRARIES}
)