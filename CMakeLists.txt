cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(openblok C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Build options
option(BUILD_TESTS "Build the unit tests" ON)

# Global compiler settings
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra") # TODO: multiplatform

set(SDL2PP_WITH_TTF ON)
add_subdirectory(external/libSDL2pp)


include_directories("src")
add_subdirectory(src)

if(BUILD_TESTS)
	add_subdirectory(external/unittest-cpp)
	include_directories(external/unittest-cpp)
	add_subdirectory(tests)
endif()
