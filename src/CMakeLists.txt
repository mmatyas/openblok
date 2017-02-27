# OpenBlok executable settings

configure_file(version.h.in generated/version.h @ONLY)
include_directories(openblok PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")

add_executable(openblok main.cpp version.h)

add_subdirectory(system)
add_subdirectory(game)
target_link_libraries(openblok module_game)


# Install
install(TARGETS openblok RUNTIME DESTINATION ${EXEDIR})
