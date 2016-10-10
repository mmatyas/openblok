set(MOD_GAME_SRC
	AppContext.cpp
	components/HoldQueue.cpp
	components/Mino.cpp
	components/MinoStorage.cpp
	components/NextQueue.cpp
	components/Piece.cpp
	components/PieceFactory.cpp
	components/Well.cpp
	components/animations/CellLockAnim.cpp
	components/animations/LineClearAnim.cpp
	states/InitState.cpp
	states/MainMenuState.cpp
	states/SinglePlayState.cpp
)

set(MOD_GAME_H
	AppContext.h
	GameState.h
	Matrix.h
	Timing.h
	Transition.h
	components/HoldQueue.h
	components/Mino.h
	components/MinoStorage.h
	components/NextQueue.h
	components/Piece.h
	components/PieceFactory.h
	components/PieceType.h
	components/Well.h
	components/animations/WellAnimation.h
	components/animations/CellLockAnim.h
	components/animations/LineClearAnim.h
	states/InitState.h
	states/MainMenuState.h
	states/SinglePlayState.h
)

add_library(module_game ${MOD_GAME_SRC} ${MOD_GAME_H})
target_link_libraries(module_game module_system)

# std::future support
target_link_libraries(module_game ${CMAKE_THREAD_LIBS_INIT})
