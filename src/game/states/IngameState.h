#pragma once

#include "game/GameState.h"
#include "game/PlayerStatistics.h"
#include "game/layout/gameplay/PlayerArea.h"

#include <list>
#include <memory>
#include <unordered_map>

namespace SubStates { namespace Ingame {
    class State;
} }


enum class GameMode : uint8_t {
    SP_MARATHON,
    SP_40LINES,
    MP_MARATHON,
    MP_BATTLE,
};

bool isSinglePlayer(GameMode);

class IngameState: public GameState {
public:
    IngameState(AppContext&, GameMode);
    ~IngameState();

    void update(const std::vector<Event>&, AppContext&) final;
    void draw(GraphicsContext&) final;

    void updatePositions(GraphicsContext&);

    const GameMode gamemode;
    std::list<std::unique_ptr<SubStates::Ingame::State>> states;
    std::vector<DeviceID> device_order;
    std::unordered_map<DeviceID, Layout::PlayerArea> player_areas;
    std::unordered_map<DeviceID, PlayerStatistics> player_stats;

    const float draw_scale;
    const float draw_inverse_scale;

private:
    std::unique_ptr<Texture> tex_background;

    void drawCommon(GraphicsContext&);
};
