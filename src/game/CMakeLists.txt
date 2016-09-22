set(MOD_GAME_SRC
	AppContext.cpp
	CommonResources.cpp
	GameState.cpp
	components/HoldQueue.cpp
	components/Mino.cpp
	components/MinoFactory.cpp
	components/NextQueue.cpp
	components/Piece.cpp
	components/PieceFactory.cpp
	components/Well.cpp
	states/InitState.cpp
	states/MainMenuState.cpp
	states/SinglePlayState.cpp
)

set(MOD_GAME_H
	AppContext.h
	CommonResources.h
	GameState.h
	Matrix.h
	Transition.h
	components/HoldQueue.h
	components/Mino.h
	components/MinoFactory.h
	components/NextQueue.h
	components/Piece.h
	components/PieceFactory.h
	components/Well.h
	states/InitState.h
	states/MainMenuState.h
	states/SinglePlayState.h
)

add_library(module_game ${MOD_GAME_SRC} ${MOD_GAME_H})
target_link_libraries(module_game module_system)

# std::future support
target_link_libraries(module_game ${CMAKE_THREAD_LIBS_INIT})
