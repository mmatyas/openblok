cmake_minimum_required(VERSION 2.8.1)
project(UnitTest++)

option(UTPP_USE_PLUS_SIGN
    "Set this to OFF if you wish to use '-cpp' instead of '++' in lib/include paths"
    ON)
option(UTPP_INCLUDE_TESTS_IN_BUILD
    "Set this to OFF if you do not wish to automatically build or run unit tests as part of the default cmake --build"
    ON)
option(UTPP_AMPLIFY_WARNINGS
    "Set this to OFF if you wish to use CMake default warning levels; should generally only use to work around support issues for your specific compiler"
    ON)

set(LIB_SUFFIX "" CACHE STRING "Identifier to add to end of lib directory name e.g. 64 for lib64")

# get the main sources
file(GLOB headers_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} UnitTest++/*.h)
file(GLOB sources_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} UnitTest++/*.cpp)
source_group("" FILES ${headers_} ${sources_})

# get platform specific sources
if (WIN32)
    target_add_definitions( PRIVATE -D_CRT_SECURE_NO_DEPRECATE)
    set(platformDir_ Win32)
else()
    set(platformDir_ Posix)
endif(WIN32)

file(GLOB platformHeaders_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} UnitTest++/${platformDir_}/*.h)
file(GLOB platformSources_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} UnitTest++/${platformDir_}/*.cpp)
source_group(${platformDir_} FILES ${platformHeaders_} ${platformSources_})

# create the lib
add_library(UnitTest++ STATIC ${headers_} ${sources_} ${platformHeaders_} ${platformSources_})
set_target_properties(UnitTest++ PROPERTIES CXX_STANDARD 14)
set_target_properties(UnitTest++ PROPERTIES CXX_EXTENSIONS NO)

# up warning level for project
if (${UTPP_AMPLIFY_WARNINGS})
    if (WIN32)
        target_compile_options(UnitTest++ PRIVATE /W4 /WX)
    else()
        target_compile_options(UnitTest++ PRIVATE -Wall -Wextra -Werror)
    endif()
endif()

if(${UTPP_USE_PLUS_SIGN})
	set_target_properties(UnitTest++ PROPERTIES OUTPUT_NAME UnitTest++)
endif()


# build the test runner
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.cpp tests/*.h)
source_group( "" FILES ${TEST_SRCS})
add_executable(TestUnitTest++ ${TEST_SRCS})

if(${UTPP_USE_PLUS_SIGN})
	set_target_properties(TestUnitTest++ PROPERTIES OUTPUT_NAME TestUnitTest++)
endif()

target_link_libraries(TestUnitTest++ UnitTest++)

set_target_properties(TestUnitTest++ PROPERTIES CXX_STANDARD 14)
set_target_properties(TestUnitTest++ PROPERTIES CXX_EXTENSIONS NO)

# run unit tests as post build step
add_custom_command(TARGET TestUnitTest++
    POST_BUILD COMMAND TestUnitTest++
    COMMENT "Running unit tests")

if(NOT ${UTPP_INCLUDE_TESTS_IN_BUILD})
    set_target_properties(TestUnitTest++ PROPERTIES EXCLUDE_FROM_ALL 1)
endif()

# add install targets
# need a custom install path?
# define CMAKE_INSTALL_PREFIX to change root folder
if(${UTPP_USE_PLUS_SIGN})
	set (UTPP_INSTALL_DESTINATION "include/UnitTest++")
else()
	set (UTPP_INSTALL_DESTINATION "include/UnitTestPP")
endif()
target_include_directories(UnitTest++ PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${UTPP_INSTALL_DESTINATION}>
)

set(config_install_dir_ lib${LIB_SUFFIX}/cmake/${PROJECT_NAME})
set(targets_export_name_ "${PROJECT_NAME}Targets")

install(TARGETS UnitTest++ EXPORT "${targets_export_name_}" DESTINATION lib${LIB_SUFFIX})
install(FILES ${headers_} DESTINATION ${UTPP_INSTALL_DESTINATION})
install(FILES ${platformHeaders_} DESTINATION ${UTPP_INSTALL_DESTINATION}/${platformDir_})
install(FILES cmake/UnitTest++Config.cmake DESTINATION "${config_install_dir_}")
install(EXPORT "${targets_export_name_}" DESTINATION "${config_install_dir_}")

set(prefix      ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX}/bin)
set(libdir      ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(includedir  ${CMAKE_INSTALL_PREFIX}/include/UnitTest++)
configure_file("UnitTest++.pc.in" "UnitTest++.pc" @ONLY)
if(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    set(pkgconfdir  ${CMAKE_INSTALL_PREFIX}/libdata/pkgconfig)
else()
    set(pkgconfdir  ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/pkgconfig)
endif()
install(FILES       "${CMAKE_CURRENT_BINARY_DIR}/UnitTest++.pc"
        DESTINATION "${pkgconfdir}")
