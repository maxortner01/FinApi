cmake_minimum_required(VERSION 2.8.9)

project(finapi)
set(CMAKE_BUILD_TYPE Release)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(finapi STATIC ${SOURCES})

find_library(PTHREAD_LIB pthread)
target_link_libraries(finapi "${PTHREAD_LIB}")

install(TARGETS finapi DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}../lib)