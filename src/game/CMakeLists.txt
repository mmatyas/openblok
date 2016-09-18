set(MOD_GAME_SRC
	AppContext.cpp
	GameState.cpp
	components/Holder.cpp
	components/Mino.cpp
	components/MinoFactory.cpp
	components/Piece.cpp
	components/PieceFactory.cpp
	components/PieceQueue.cpp
	components/Well.cpp
	states/InitState.cpp
	states/MainMenuState.cpp
	states/SinglePlayState.cpp
)

set(MOD_GAME_H
	Animation.h
	AppContext.h
	GameState.h
	Matrix.h
	Resources.h
	components/Holder.h
	components/Mino.h
	components/MinoFactory.h
	components/Piece.h
	components/PieceFactory.h
	components/PieceQueue.h
	components/Well.h
	states/InitState.h
	states/MainMenuState.h
	states/SinglePlayState.h
)

add_library(module_game ${MOD_GAME_SRC} ${MOD_GAME_H})
target_link_libraries(module_game module_system)

# std::future support
target_link_libraries(module_game ${CMAKE_THREAD_LIBS_INIT})
