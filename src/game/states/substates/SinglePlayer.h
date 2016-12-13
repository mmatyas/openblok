#pragma once

#include "system/Event.h"

#include <vector>

class AppContext;
class GraphicsContext;
class SinglePlayState;


namespace SubStates {
namespace SinglePlayer {

class State {
public:
    virtual ~State() {}
    virtual void updateAnimationsOnly(SinglePlayState&, AppContext&) {}
    virtual void update(SinglePlayState&, const std::vector<Event>&, AppContext&) = 0;
    virtual void draw(SinglePlayState&, GraphicsContext&) const {}
};

} // namespace SinglePlayer
} // namespace SubStates
