#pragma once

#include "system/Event.h"

#include <vector>

class AppContext;
class GraphicsContext;
class IngameState;


namespace SubStates {
namespace Ingame {

class State {
public:
    virtual ~State() {}
    virtual void updateAnimationsOnly(IngameState&, AppContext&) {}
    virtual void update(IngameState&, const std::vector<Event>&, AppContext&) = 0;
    virtual void drawPassive(IngameState&, GraphicsContext&) const {}
    virtual void drawActive(IngameState&, GraphicsContext&) const {}
};

} // namespace Ingame
} // namespace SubStates
