cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(openblok C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(ProjectVersion)


# Build options

include(SetDefaultBuildType)

# Portable install
set(INSTALL_PORTABLE_DEFAULT OFF)
if(WIN32 OR CYGWIN OR VITA)
    set(INSTALL_PORTABLE_DEFAULT ON)
endif()
option(INSTALL_PORTABLE "The installation step should put the data directory next to the runtime" ${INSTALL_PORTABLE_DEFAULT})

# Currently unit tests only work only in Debug
if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "release" AND NOT VITA)
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
    if(UNIX AND NOT APPLE AND NOT CYGWIN AND NOT VITA)
        option(INSTALL_DESKTOPICON "Install desktop shortcut and icon" ON)
        option(INSTALL_APPSTREAM "Install AppStream metainfo" ON)
        set(ICONDIR "${SHAREDIR}/pixmaps" CACHE STRING "Install location of the icon file")
        set(DESKTOPDIR "${SHAREDIR}/applications" CACHE STRING "Install location of the desktop shortcut")
        set(APPSTREAMDIR "${SHAREDIR}/metainfo" CACHE STRING "Install location of the AppStream metainfo")
    endif()
endif()

# Localization
set(ENABLE_LOCALES_DEFAULT ON)
if(VITA)
    set(ENABLE_LOCALES_DEFAULT OFF)
endif()
option(ENABLE_LOCALES "Enable localization support" ${ENABLE_LOCALES_DEFAULT})
if(ENABLE_LOCALES)
    add_subdirectory(locale)
endif()


# Global compiler settings

# Build coverage report if requested
if(BUILD_TEST_COVERAGE)
    include(CodeCoverage)
    setup_target_for_coverage(openblok_coverage openblok_test coverage)
endif()


add_subdirectory(thirdparty)


# The main game source
include_directories(src)
add_subdirectory(src)
if(CMAKE_BUILD_TYPE STREQUAL "debug" AND BUILD_TESTS)
    add_subdirectory(tests)
endif()


# Install
install(DIRECTORY data/ DESTINATION ${DATADIR} PATTERN "*.txt" EXCLUDE)
if(INSTALL_DESKTOPICON)
    install(FILES etc/linux/hu.mmatyas.openblok.desktop DESTINATION ${DESKTOPDIR})
    install(FILES data/icon.png DESTINATION ${ICONDIR} RENAME openblok.png)
endif()
if(INSTALL_APPSTREAM)
    install(FILES etc/linux/hu.mmatyas.openblok.metainfo.xml DESTINATION ${APPSTREAMDIR})
endif()


# Package
include(CPackConfig)
include(CPack)


# Display settings
set(MSG_BUILDTYPE ${CMAKE_BUILD_TYPE})
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
if(INSTALL_APPSTREAM)
    message(STATUS "|  - appstream:      yes")
endif()
if(ENABLE_LOCALES)
    message(STATUS "|  - localization:   yes")
endif()
message(STATUS "|")
