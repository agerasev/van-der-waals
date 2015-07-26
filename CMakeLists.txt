cmake_minimum_required(VERSION 2.6)
project(van-der-waals)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
file(STRINGS "source.files" SOURCES REGEX .c$|.cpp$)
add_definitions(-DCL_GL_INTEROP -DCL_PROFILING)
include_directories(libcl4u)

add_executable(van-der-waals ${SOURCES})
target_link_libraries(${PROJECT_NAME} -lm -lOpenCL -lSDL2 -lGL -lGLEW)
