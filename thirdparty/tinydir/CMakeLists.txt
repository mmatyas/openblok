add_library(tinydir tinydir.h tinydir_cxx.cpp tinydir_cxx.h)
target_include_directories(tinydir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(tinydir PROPERTIES CXX_STANDARD 11)
set_target_properties(tinydir PROPERTIES CXX_EXTENSIONS OFF)
