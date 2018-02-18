# OpenBlok executable settings

configure_file(version.h.in generated/version.h @ONLY)
include_directories(openblok PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")

add_executable(openblok main.cpp version.h)

add_subdirectory(system)
add_subdirectory(game)
target_link_libraries(openblok module_game)

# use custom std::make_unique if necessary
if (NOT CXX_HAS_MAKEUNIQUE)
    target_compile_definitions(openblok PRIVATE NEED_MAKEUNIQUE)
endif()


# Install
install(TARGETS openblok RUNTIME DESTINATION ${EXEDIR})
