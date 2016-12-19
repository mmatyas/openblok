#pragma once

#include "game/GameState.h"
#include "game/PlayerStatistics.h"
#include "game/layout/multiplayer/BottomBarBox.h"
#include "game/layout/multiplayer/TopBarBox.h"
#include "game/layout/singleplayer/WellBox.h"

#include <list>
#include <memory>
#include <unordered_map>

namespace SubStates { namespace Multiplayer {
    class State;
} }

enum class MultiplayerMode : uint8_t {
    MARATHON,
    BATTLE,
};

class MultiplayerState: public GameState {
public:
    MultiplayerState(AppContext&, MultiplayerMode);
    ~MultiplayerState();

    void update(const std::vector<Event>&, AppContext&) final;
    void draw(GraphicsContext&) final;

    void updatePositions(GraphicsContext&);

    const MultiplayerMode gamemode;
    std::list<std::unique_ptr<SubStates::Multiplayer::State>> states;
    std::vector<DeviceID> device_order;
    std::unordered_map<DeviceID, Layout::WellBox> ui_wells;
    std::unordered_map<DeviceID, Layout::TopBarBox> ui_topbars;
    std::unordered_map<DeviceID, Layout::BottomBarBox> ui_bottombars;
    std::unordered_map<DeviceID, PlayerStatistics> player_stats;

private:
    std::unique_ptr<Texture> tex_background;

    void drawCommon(GraphicsContext&);
};
