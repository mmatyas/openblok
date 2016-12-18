#pragma once

#include "game/GameState.h"
#include "game/layout/multiplayer/BottomBarBox.h"
#include "game/layout/multiplayer/TopBarBox.h"
#include "game/layout/singleplayer/WellBox.h"

#include <list>
#include <memory>
#include <unordered_map>

namespace SubStates { namespace Multiplayer {
    class State;
} }


class MultiplayerState: public GameState {
public:
    MultiplayerState(AppContext&);
    ~MultiplayerState();

    void update(const std::vector<Event>&, AppContext&) final;
    void draw(GraphicsContext&) final;

    void updatePositions(GraphicsContext&);

    std::list<std::unique_ptr<SubStates::Multiplayer::State>> states;
    std::vector<DeviceID> device_order;
    std::unordered_map<DeviceID, Layout::WellBox> ui_wells;
    std::unordered_map<DeviceID, Layout::TopBarBox> ui_topbars;
    std::unordered_map<DeviceID, Layout::BottomBarBox> ui_bottombars;

private:
    std::unique_ptr<Texture> tex_background;

    void drawCommon(GraphicsContext&);
};
