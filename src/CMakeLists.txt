add_definitions(-DOPENBLOK_DATADIR="${DATADIR}")

add_executable(openblok main.cpp)

add_subdirectory(system)
add_subdirectory(game)
target_link_libraries(openblok module_game)


# Install
install(TARGETS openblok RUNTIME DESTINATION ${EXEDIR})
