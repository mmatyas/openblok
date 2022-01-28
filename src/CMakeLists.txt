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


# Install
install(TARGETS openblok RUNTIME DESTINATION ${EXEDIR})

if(VITA)
    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

    vita_create_self(openblok.self openblok)

    vita_create_vpk(openblok.vpk "OPBL00001" openblok.self
        VERSION "01.00"
        NAME OpenBlok
        FILE ${CMAKE_SOURCE_DIR}/etc/vita/sce_sys sce_sys
        FILE ${CMAKE_SOURCE_DIR}/data data
    )
endif()
