set(MOD_GAME_SRC
	AppContext.cpp

	components/HoldQueue.cpp
	components/Mino.cpp
	components/MinoStorage.cpp
	components/NextQueue.cpp
	components/Piece.cpp
	components/PieceFactory.cpp
	components/PieceType.cpp
	components/Well.cpp

	components/animations/CellLockAnim.cpp
	components/animations/HalfHeightLineClearAnim.cpp
	components/animations/LineClearAnim.cpp
	components/animations/TextPopup.cpp

	components/well/AutoRepeat.cpp
	components/well/Gravity.cpp
	components/well/Input.cpp
	components/well/LockDelay.cpp
	components/well/TSpin.cpp
	components/well/Render.cpp

	layout/singleplayer/LeftSidebarBox.cpp
	layout/singleplayer/RightSidebarBox.cpp
	layout/singleplayer/WellBox.cpp

	script/ScriptLoader.cpp
	script/lua/LuaRotationFn.cpp

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
	WellEvent.h

	components/HoldQueue.h
	components/Mino.h
	components/MinoStorage.h
	components/NextQueue.h
	components/Piece.h
	components/PieceFactory.h
	components/PieceType.h
	components/Well.h
	components/WellConfig.h

	components/animations/WellAnimation.h
	components/animations/CellLockAnim.h
	components/animations/HalfHeightLineClearAnim.h
	components/animations/LineClearAnim.h
	components/animations/TextPopup.h

	components/well/AutoRepeat.h
	components/well/Gravity.h
	components/well/Input.h
	components/well/LockDelay.h
	components/well/TSpin.h
	components/well/Render.h

	layout/singleplayer/Box.h
	layout/singleplayer/LeftSidebarBox.h
	layout/singleplayer/RightSidebarBox.h
	layout/singleplayer/WellBox.h

	script/ClassicRotationFn.h
	script/ScriptLoader.h
	script/RotationFn.h
	script/lua/LuaRotationFn.h

	states/InitState.h
	states/MainMenuState.h
	states/SinglePlayState.h
)

if(NOT CMAKE_BUILD_TYPE MATCHES Release)
    list(APPEND MOD_GAME_SRC components/well/Ascii.cpp)
    list(APPEND MOD_GAME_H components/well/Ascii.h)
endif()

# use Lua
include_directories(${LUA_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/external/sol)

add_library(module_game ${MOD_GAME_SRC} ${MOD_GAME_H})
target_link_libraries(module_game module_system)
target_link_libraries(module_game ${LUA_LIBRARY})
