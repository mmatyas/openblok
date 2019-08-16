# SDL2
set(SDL2PP_CXXSTD 11)
set(SDL2PP_WITH_IMAGE ON)
set(SDL2PP_WITH_MIXER ON)
set(SDL2PP_WITH_TTF ON)
add_subdirectory(libSDL2pp)


# tinydir
add_subdirectory(tinydir)


# UnitTest++
if(CMAKE_BUILD_TYPE STREQUAL "debug" AND BUILD_TESTS)
    add_subdirectory(unittest-cpp)
endif()
