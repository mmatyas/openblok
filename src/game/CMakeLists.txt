set(MOD_GAME_SRC
    AppContext.cpp
    BattleAttackTable.cpp
    GameConfigFile.cpp
    ScoreTable.cpp
    Theme.cpp

    components/HoldQueue.cpp
    components/Mino.cpp
    components/MinoStorage.cpp
    components/NextQueue.cpp
    components/NextQueueDraw.cpp
    components/Piece.cpp
    components/PieceDraw.cpp
    components/PieceFactory.cpp
    components/PieceType.cpp
    components/Well.cpp

    components/animations/BattleAttack.cpp
    components/animations/CellLockAnim.cpp
    components/animations/HalfHeightLineClearAnim.cpp
    components/animations/LineClearAnim.cpp
    components/animations/TextPopup.cpp

    components/rotations/Classic.cpp
    components/rotations/RotationFactory.cpp
    components/rotations/SRS.cpp
    components/rotations/TGM.cpp

    components/well/AutoRepeat.cpp
    components/well/Gravity.cpp
    components/well/Input.cpp
    components/well/LockDelay.cpp
    components/well/TSpin.cpp
    components/well/Render.cpp

    layout/gameplay/GarbageGauge.cpp
    layout/gameplay/PlayerArea.cpp
    layout/gameplay/WellContainer.cpp
    layout/menu/Logo.cpp
    layout/menu/MainMenuBtn.cpp
    layout/menu/PieceRain.cpp
    layout/options/CategoryBtn.cpp
    layout/options/DeviceChooser.cpp
    layout/options/InputField.cpp
    layout/options/OptionsItem.cpp
    layout/options/ToggleBtn.cpp
    layout/options/ValueChooser.cpp

    states/InitState.cpp
    states/IngameState.cpp
    states/MainMenuState.cpp
    states/substates/ingame/Countdown.cpp
    states/substates/ingame/FadeInOut.cpp
    states/substates/ingame/Gameplay.cpp
    states/substates/ingame/Pause.cpp
    states/substates/ingame/PlayerSelect.cpp
    states/substates/ingame/Statistics.cpp
    states/substates/mainmenu/Base.cpp
    states/substates/mainmenu/Options.cpp

    util/DurationToString.cpp
)

set(MOD_GAME_H
    AppContext.h
    BattleAttackTable.h
    GameConfigFile.h
    GameState.h
    PlayerStatistics.h
    ScoreTable.h
    SysConfig.h
    Theme.h
    Timing.h
    Transition.h
    WellConfig.h
    WellEvent.h

    components/HoldQueue.h
    components/Mino.h
    components/MinoStorage.h
    components/NextQueue.h
    components/NextQueueDraw.h
    components/Piece.h
    components/PieceDraw.h
    components/PieceFactory.h
    components/PieceType.h
    components/Well.h

    components/animations/BattleAttack.h
    components/animations/CellLockAnim.h
    components/animations/HalfHeightLineClearAnim.h
    components/animations/LineClearAnim.h
    components/animations/TextPopup.h
    components/animations/WellAnimation.h

    components/rotations/Classic.h
    components/rotations/RotationFactory.h
    components/rotations/RotationFn.h
    components/rotations/RotationStyle.h
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
    layout/gameplay/GarbageGauge.h
    layout/gameplay/PlayerArea.h
    layout/gameplay/WellContainer.h
    layout/menu/Button.h
    layout/menu/Logo.h
    layout/menu/MainMenuBtn.h
    layout/menu/PieceRain.h
    layout/options/CategoryBtn.h
    layout/options/DeviceChooser.h
    layout/options/InputField.h
    layout/options/OptionsItem.h
    layout/options/ToggleBtn.h
    layout/options/ValueChooser.h

    states/InitState.h
    states/IngameState.h
    states/MainMenuState.h
    states/substates/Ingame.h
    states/substates/MainMenu.h
    states/substates/ingame/Countdown.h
    states/substates/ingame/FadeInOut.h
    states/substates/ingame/Gameplay.h
    states/substates/ingame/Pause.h
    states/substates/ingame/PlayerSelect.h
    states/substates/ingame/Statistics.h
    states/substates/mainmenu/Base.h
    states/substates/mainmenu/Options.h

    util/CircularModulo.h
    util/DurationToString.h
    util/Matrix.h
)

if(CMAKE_BUILD_TYPE STREQUAL "debug")
    list(APPEND MOD_GAME_SRC components/well/Ascii.cpp)
    list(APPEND MOD_GAME_H components/well/Ascii.h)
endif()

add_library(module_game ${MOD_GAME_SRC} ${MOD_GAME_H})
target_link_libraries(module_game module_system)
target_link_libraries(module_game tinydir)

include(EnableWarnings)
include(RequireCxxStd)
enable_warnings(module_game)
require_cxx11_or_higher(module_game)

# use Boost.Regex if requested
if (USE_BOOST_REGEX)
    target_include_directories(module_game PRIVATE ${Boost_INCLUDE_DIRS})
    target_compile_definitions(module_game PRIVATE USE_BOOST_REGEX)
    target_link_libraries(module_game ${Boost_LIBRARIES})
endif()

# use custom std::make_unique if necessary
if (NOT CXX_HAS_MAKEUNIQUE)
    target_compile_definitions(module_game PRIVATE NEED_MAKEUNIQUE)
endif()
