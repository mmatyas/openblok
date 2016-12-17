#pragma once

#include "game/Transition.h"
#include "game/states/substates/Multiplayer.h"


namespace SubStates {
namespace Multiplayer {
namespace States {

class FadeIn : public State {
public:
    FadeIn(std::function<void()>&& on_end);
    void update(MultiplayerState&, const std::vector<Event>&, AppContext&) final;
    void draw(MultiplayerState&, GraphicsContext&) const final;

private:
    Transition<uint8_t> alpha;
};

class FadeOut : public State {
public:
    FadeOut(std::function<void()>&& on_end);
    void update(MultiplayerState&, const std::vector<Event>&, AppContext&) final;
    void draw(MultiplayerState&, GraphicsContext&) const final;

private:
    Transition<uint8_t> alpha;
};

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
