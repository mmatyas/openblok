#pragma once

#include "game/Transition.h"
#include "game/states/substates/Ingame.h"


namespace SubStates {
namespace Ingame {
namespace States {

class FadeIn : public State {
public:
    FadeIn(std::function<void()>&& on_end, float scale = 1.f);
    void update(IngameState&, const std::vector<Event>&, AppContext&) final;
    void drawActive(IngameState&, GraphicsContext&) const final;

private:
    const float scale;
    Transition<uint8_t> alpha;
};

class FadeOut : public State {
public:
    FadeOut(std::function<void()>&& on_end, float scale = 1.f);
    void update(IngameState&, const std::vector<Event>&, AppContext&) final;
    void drawActive(IngameState&, GraphicsContext&) const final;

private:
    const float scale;
    Transition<uint8_t> alpha;
};

} // namespace States
} // namespace Ingame
} // namespace SubStates
