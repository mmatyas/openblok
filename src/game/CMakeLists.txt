set(MOD_GAME_SRC
	AppContext.cpp
	InitState.cpp
)

set(MOD_GAME_H
	AppContext.h
	GameState.h
	InitState.h
)

add_library(module_game ${MOD_GAME_SRC} ${MOD_GAME_H})
target_link_libraries(module_game module_system)
