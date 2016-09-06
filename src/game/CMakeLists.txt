set(MOD_GAME_SRC
	AppContext.cpp
	Mino.cpp
	MinoFactory.cpp
	Piece.cpp
	PieceFactory.cpp
	states/InitState.cpp
	states/MainMenuState.cpp
	states/SinglePlayState.cpp
)

set(MOD_GAME_H
	AppContext.h
	GameState.h
	Matrix.h
	Mino.h
	MinoFactory.h
	Piece.h
	PieceFactory.h
	Resources.h
	states/InitState.h
	states/MainMenuState.h
	states/SinglePlayState.h
)

add_library(module_game ${MOD_GAME_SRC} ${MOD_GAME_H})
target_link_libraries(module_game module_system)

# std::future support
target_link_libraries(module_game ${CMAKE_THREAD_LIBS_INIT})
