#pragma once

#include "game/GameState.h"
#include "game/PlayerStatistics.h"

#include <list>
#include <memory>
#include <unordered_map>

namespace Layout {
    class PlayerArea;
}

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
    std::unordered_map<DeviceID, std::unique_ptr<Layout::PlayerArea>> player_areas;
    std::unordered_map<DeviceID, PlayerStatistics> player_stats;

private:
    std::unique_ptr<Texture> tex_background;

    void drawCommon(GraphicsContext&);
};
