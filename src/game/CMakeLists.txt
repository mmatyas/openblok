set(MOD_GAME_SRC
    AppContext.cpp
    ScoreTable.cpp

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

    components/rotations/Classic.cpp
    components/rotations/SRS.cpp
    components/rotations/TGM.cpp

    components/well/AutoRepeat.cpp
    components/well/Gravity.cpp
    components/well/Input.cpp
    components/well/LockDelay.cpp
    components/well/TSpin.cpp
    components/well/Render.cpp

    layout/menu/Logo.cpp
    layout/menu/MainMenuBtn.cpp
    layout/menu/PieceRain.cpp
    layout/options/CategoryBtn.cpp
    layout/options/OptionsItem.cpp
    layout/options/ToggleBtn.cpp
    layout/singleplayer/LeftSidebarBox.cpp
    layout/singleplayer/RightSidebarBox.cpp
    layout/singleplayer/WellBox.cpp

    states/InitState.cpp
    states/MainMenuState.cpp
    states/SinglePlayState.cpp
    states/substates/mainmenu/Base.cpp
    states/substates/mainmenu/Options.cpp
    states/substates/singleplayer/Gameplay.cpp
    states/substates/singleplayer/MinorStates.cpp
    states/substates/singleplayer/Statistics.cpp

    util/DurationToString.cpp
)

set(MOD_GAME_H
    AppContext.h
    GameState.h
    PlayerStatistics.h
    ScoreTable.h
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

    components/rotations/Classic.h
    components/rotations/RotationFn.h
    components/rotations/SRS.h
    components/rotations/TGM.h

    components/well/AutoRepeat.h
    components/well/Gravity.h
    components/well/Input.h
    components/well/LockDelay.h
    components/well/TSpin.h
    components/well/Render.h

    layout/Box.h
    layout/MenuItem.h
    layout/menu/Button.h
    layout/menu/Logo.h
    layout/menu/MainMenuBtn.h
    layout/menu/PieceRain.h
    layout/options/CategoryBtn.h
    layout/options/OptionsItem.h
    layout/options/ToggleBtn.h
    layout/singleplayer/LeftSidebarBox.h
    layout/singleplayer/RightSidebarBox.h
    layout/singleplayer/WellBox.h

    states/InitState.h
    states/MainMenuState.h
    states/SinglePlayState.h
    states/substates/SinglePlayer.h
    states/substates/MainMenu.h
    states/substates/mainmenu/Base.h
    states/substates/mainmenu/Options.h
    states/substates/singleplayer/Gameplay.h
    states/substates/singleplayer/MinorStates.h
    states/substates/singleplayer/Statistics.h

    util/CircularModulo.h
    util/DurationToString.h
    util/Matrix.h
)

if(NOT CMAKE_BUILD_TYPE MATCHES Release)
    list(APPEND MOD_GAME_SRC components/well/Ascii.cpp)
    list(APPEND MOD_GAME_H components/well/Ascii.h)
endif()

add_library(module_game ${MOD_GAME_SRC} ${MOD_GAME_H})
target_link_libraries(module_game module_system)
