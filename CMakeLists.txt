cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(openblok C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Build options
option(BUILD_TESTS "Build the unit tests" ON)

# Global compiler settings
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra") # TODO: multiplatform

# Dependencies: SDL2
set(SDL2PP_WITH_IMAGE ON)
set(SDL2PP_WITH_TTF ON)
add_subdirectory(external/libSDL2pp)

# Dependencies: C++11 std::future support
find_package(Threads REQUIRED)


include_directories("src")
add_subdirectory(src)

if(BUILD_TESTS)
	add_subdirectory(external/unittest-cpp)
	include_directories(external/unittest-cpp)
	add_subdirectory(tests)
endif()
