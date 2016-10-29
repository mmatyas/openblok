cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(openblok C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Build options
option(BUILD_TESTS "Build the unit tests" ON)
option(BUILD_TEST_COVERAGE "Build the test coverage report" OFF)
option(WITH_LUAJIT "Use LuaJIT instead of default Lua" ON)

# Global compiler settings
# Use C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Turn on warnings
include(CheckCXXCompilerFlag)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
else()
    check_cxx_compiler_flag("-Wall -Wextra -pedantic" OBLOK_WARNING_FLAGS_SUPPORTED)
    if(OBLOK_WARNING_FLAGS_SUPPORTED)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    endif()
endif()
# Build coverage report if requested
if(BUILD_TEST_COVERAGE)
	include(CodeCoverage)
	setup_target_for_coverage(openblok_coverage openblok_test coverage)
endif()


# Dependencies: SDL2
set(SDL2PP_CXXSTD "c++14")
set(SDL2PP_WITH_IMAGE ON)
set(SDL2PP_WITH_MIXER ON)
set(SDL2PP_WITH_TTF ON)
add_subdirectory(external/libSDL2pp)

# Dependencies: Lua
find_package(Lua REQUIRED)


include_directories("src")
add_subdirectory(src)

if(BUILD_TESTS)
	add_subdirectory(external/unittest-cpp)
	include_directories(external/unittest-cpp)
	add_subdirectory(tests)
endif()
