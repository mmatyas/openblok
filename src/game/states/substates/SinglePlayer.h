#pragma once

#include "game/Transition.h"
#include "system/Event.h"

#include <vector>


class AppContext;
class GraphicsContext;
class SinglePlayState;


namespace SubStates {
namespace SinglePlayer {

enum class StateType : uint8_t {
    FADE_IN,
    COUNTDOWN,
    GAME_RUNNING,
    PAUSED,
    FINISHED,
};

class State {
public:
    virtual ~State() {}
    virtual StateType type() const = 0;
    virtual void update(SinglePlayState&, const std::vector<Event>& events, AppContext&) = 0;
    virtual void draw(SinglePlayState&, GraphicsContext&) const {};
};

namespace States {

class FadeIn : public State {
public:
    FadeIn();
    StateType type() const { return StateType::FADE_IN; }
    void update(SinglePlayState&, const std::vector<Event>& events, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    Transition<uint8_t> alpha;
};

class Countdown : public State {
public:
    Countdown(SinglePlayState& parent);
    StateType type() const { return StateType::COUNTDOWN; }
    void update(SinglePlayState&, const std::vector<Event>& events, AppContext&) final;

private:
    uint8_t current_idx;
    Transition<void> timer;
};

class Gameplay : public State {
public:
    Gameplay();
    StateType type() const { return StateType::GAME_RUNNING; }
    void update(SinglePlayState&, const std::vector<Event>& events, AppContext&) final;
};

class Pause : public State {
public:
    Pause(AppContext&);
    StateType type() const { return StateType::PAUSED; }
    void update(SinglePlayState&, const std::vector<Event>& events, AppContext&) final;
};

} // namespace States
} // namespace SinglePlayer
} // namespace SubStates
