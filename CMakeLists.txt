cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(openblok C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Build options
# Currently unit tests only work only in Debug
if(NOT ${CMAKE_BUILD_TYPE} MATCHES Release)
    option(BUILD_TESTS "Build the unit tests" ON)
    option(BUILD_TEST_COVERAGE "Build the test coverage report" OFF)
endif()
# Intallation locations
set(SHAREDIR "${CMAKE_INSTALL_PREFIX}/share")
if(UNIX AND NOT APPLE AND NOT CYGWIN)
    option(INSTALL_DESKTOPICON "Install desktop shortcut and icon" ON)
    set(ICONDIR "${SHAREDIR}/pixmaps" CACHE STRING "Install location of the icon file")
    set(DESKTOPDIR "${SHAREDIR}/applications" CACHE STRING "Install location of the desktop shortcut")
endif()
if(CMAKE_BUILD_TYPE MATCHES Release)
    set(DATADIR "${SHAREDIR}/openblok" CACHE STRING "Install location of the data files (release)")
else()
    set(DATADIR "data" CACHE STRING "Install location of the data files (debug)")
endif()


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
    check_cxx_compiler_flag("-Wno-missing-braces" OBLOK_DISABLE_MISSING_BRACES_WARNING)
    if(OBLOK_DISABLE_MISSING_BRACES_WARNING)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces")
    endif()
endif()
# Build coverage report if requested
if(BUILD_TEST_COVERAGE)
	include(CodeCoverage)
	setup_target_for_coverage(openblok_coverage openblok_test coverage)
endif()


# Dependencies: SDL2
set(SDL2PP_CXXSTD "c++14" CACHE STRING "libSDL2pp C++ standard")
set(SDL2PP_WITH_IMAGE ON)
set(SDL2PP_WITH_MIXER ON)
set(SDL2PP_WITH_TTF ON)
add_subdirectory(external/libSDL2pp)


# Data files
add_definitions(-DOPENBLOK_DATADIR="${DATADIR}")


include_directories("src")
add_subdirectory(src)

if(BUILD_TESTS)
	add_subdirectory(external/unittest-cpp)
	include_directories(external/unittest-cpp)
	add_subdirectory(tests)
endif()


# Install
install(DIRECTORY data/ DESTINATION ${DATADIR} PATTERN "*.txt" EXCLUDE)
if(INSTALL_DESKTOPICON)
    install(FILES etc/linux/openblok.desktop DESTINATION ${DESKTOPDIR})
    install(FILES data/icon.png DESTINATION ${ICONDIR} RENAME openblok.png)
endif()


# Package
include(cmake/CPackConfig.cmake)
include(CPack)
