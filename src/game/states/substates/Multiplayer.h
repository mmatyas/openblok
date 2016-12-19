#pragma once

#include "system/Event.h"

#include <vector>

class AppContext;
class GraphicsContext;
class MultiplayerState;


namespace SubStates {
namespace Multiplayer {

class State {
public:
    virtual ~State() {}
    virtual void updateAnimationsOnly(MultiplayerState&, AppContext&) {}
    virtual void update(MultiplayerState&, const std::vector<Event>&, AppContext&) = 0;
    virtual void drawPassive(MultiplayerState&, GraphicsContext&) const {}
    virtual void drawActive(MultiplayerState&, GraphicsContext&) const {}
};

} // namespace Multiplayer
} // namespace SubStates

