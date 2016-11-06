set(GAME_SRC
	main.cpp
)

add_executable(openblok ${GAME_SRC})

add_subdirectory(system)
add_subdirectory(game)
target_link_libraries(openblok module_game)


# Install
install(TARGETS openblok RUNTIME DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/ DESTINATION share/openblok PATTERN "*.txt" EXCLUDE)
