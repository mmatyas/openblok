# OpenBlok executable settings
add_executable(openblok main.cpp)

configure_file(version.h.in generated/version.h @ONLY)
target_include_directories(openblok PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")

add_subdirectory(system)
add_subdirectory(game)
target_link_libraries(openblok module_game)

include(EnableWarnings)
include(RequireCxx11)
enable_warnings(openblok)
require_cxx11_or_higher(openblok)

if(WIN32 OR CYGWIN)
    include(TryAddingCompilerFlag)
    try_adding_compiler_flag(openblok -Wl,--subsystem,windows)
endif()

file(COPY po DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Install
install(TARGETS openblok RUNTIME DESTINATION ${EXEDIR})
