set(GAME_SRC
	main.cpp
)

add_executable(openblok ${GAME_SRC})

include_directories(".")

add_subdirectory(system)
target_link_libraries(openblok module_system)
