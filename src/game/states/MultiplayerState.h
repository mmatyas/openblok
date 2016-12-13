#pragma once

#include "game/GameState.h"

#include <list>
#include <memory>

namespace SubStates { namespace Multiplayer {
    class State;
} }


class MultiplayerState: public GameState {
public:
    MultiplayerState(AppContext&);
    ~MultiplayerState();

    void update(const std::vector<Event>&, AppContext&) final;
    void draw(GraphicsContext&) final;

    std::list<std::unique_ptr<SubStates::Multiplayer::State>> states;

private:
    std::unique_ptr<Texture> tex_background;

    void updatePositions(GraphicsContext&);
    void drawCommon(GraphicsContext&);
};
