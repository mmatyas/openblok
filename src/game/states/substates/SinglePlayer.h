#pragma once

#include "system/Event.h"

#include <vector>

class AppContext;
class GraphicsContext;
class SinglePlayState;


namespace SubStates {
namespace SinglePlayer {

enum class StateType : uint8_t {
    FADE_IN,
    FADE_OUT,
    COUNTDOWN,
    GAME_RUNNING,
    PAUSED,
    GAME_OVER,
    GAME_COMPLETE,
    STATISTICS,
};

class State {
public:
    virtual ~State() {}
    virtual StateType type() const = 0;
    virtual void updateAnimationsOnly(SinglePlayState&, AppContext&) {}
    virtual void update(SinglePlayState&, const std::vector<Event>&, AppContext&) = 0;
    virtual void draw(SinglePlayState&, GraphicsContext&) const {}
};

} // namespace SinglePlayer
} // namespace SubStates
