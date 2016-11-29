cmake_minimum_required(VERSION 2.8)
project(openblok C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(ProjectVersion)

# Build options
option(INSTALL_PORTABLE "The installation step should put the runtime and the data directory next to each other" OFF)
# Currently unit tests only work only in Debug
if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    option(BUILD_TESTS "Build the unit tests" ON)
    option(BUILD_TEST_COVERAGE "Build the test coverage report" OFF)
endif()
# Intallation locations
if(INSTALL_PORTABLE)
    set(EXEDIR "." CACHE STRING "Install location of the runtime executable")
    set(DATADIR "./data" CACHE STRING "Install location of the data files")
else()
    set(SHAREDIR "${CMAKE_INSTALL_PREFIX}/share" CACHE STRING "Base directory of installed data files")
    set(EXEDIR "${CMAKE_INSTALL_PREFIX}/games" CACHE STRING "Install location of the runtime executable")
    set(DATADIR "${SHAREDIR}/openblok" CACHE STRING "Install location of the data files")
    if(UNIX AND NOT APPLE AND NOT CYGWIN)
        option(INSTALL_DESKTOPICON "Install desktop shortcut and icon" ON)
        set(ICONDIR "${SHAREDIR}/pixmaps" CACHE STRING "Install location of the icon file")
        set(DESKTOPDIR "${SHAREDIR}/applications" CACHE STRING "Install location of the desktop shortcut")
    endif()
endif()


# Global compiler settings
# Use C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_VERSION VERSION_LESS 3.1.0)
    if(NOT MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    endif()
endif()
# Turn on warnings
include(TryAddingCompilerFlag)
if(MSVC)
    try_adding_compiler_flag(/W3) # warnings
    try_adding_compiler_flag(/Za) # disables lang extensions, enables boolean keywords
else()
    try_adding_compiler_flag(-Wall)
    try_adding_compiler_flag(-Wextra)
    try_adding_compiler_flag(-pedantic)
    if(WIN32 OR CYGWIN)
        try_adding_compiler_flag(-Wl,--subsystem,windows)
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


# The main game source
include_directories(src)
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
include(CPackConfig)
include(CPack)


# Display settings
set(MSG_BUILDTYPE ${CMAKE_BUILD_TYPE})
if(NOT MSG_BUILDTYPE)
    set(MSG_BUILDTYPE "Default/Not optimized")
endif()
set(MSG_TESTS "do not build")
if(BUILD_TESTS)
    set(MSG_TESTS "build, tests only")
    if(BUILD_TEST_COVERAGE)
        set(MSG_TESTS "build, with coverage")
    endif()
endif()
set(MSG_INSTALL "install to ${CMAKE_INSTALL_PREFIX}")
if(INSTALL_PORTABLE)
    set(MSG_INSTALL "portable, default ${MSG_INSTALL}")
endif()
message(STATUS "|")
message(STATUS "|  Build type:       ${MSG_BUILDTYPE}")
message(STATUS "|  Tests:            ${MSG_TESTS}")
message(STATUS "|  Install:          ${MSG_INSTALL}")
message(STATUS "|  - runtime dir:    ${EXEDIR}")
message(STATUS "|  - data dir:       ${DATADIR}")
if(INSTALL_DESKTOPICON)
    message(STATUS "|  - shortcut, icon: yes")
endif()
message(STATUS "|")
