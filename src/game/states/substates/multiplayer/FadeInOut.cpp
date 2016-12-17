#include "FadeInOut.h"

#include "game/states/MultiplayerState.h"


namespace SubStates {
namespace Multiplayer {
namespace States {

FadeIn::FadeIn(std::function<void()>&& on_end)
    : alpha(std::chrono::milliseconds(300),
            [](double t){ return (1.0 - t) * 0xFF; },
            std::move(on_end))
{}

void FadeIn::update(MultiplayerState&, const std::vector<Event>&, AppContext&)
{
    alpha.update(Timing::frame_duration);
}

void FadeIn::draw(MultiplayerState& parent, GraphicsContext& gcx) const
{
    auto it = parent.states.begin();
    auto it_end = --parent.states.end();
    for (; it != it_end; ++it)
        (*it)->draw(parent, gcx);

    RGBAColor color = 0x00_rgba;
    color.a = alpha.value();
    gcx.drawFilledRect({0, 0, gcx.screenWidth(), gcx.screenHeight()}, color);
}

FadeOut::FadeOut(std::function<void()>&& on_end)
    : alpha(std::chrono::milliseconds(300),
            [](double t){ return t * 0xFF; },
            std::move(on_end))
{}

void FadeOut::update(MultiplayerState&, const std::vector<Event>&, AppContext&)
{
    alpha.update(Timing::frame_duration);
}

void FadeOut::draw(MultiplayerState& parent, GraphicsContext& gcx) const
{
    auto it = parent.states.begin();
    auto it_end = --parent.states.end();
    for (; it != it_end; ++it)
        (*it)->draw(parent, gcx);

    RGBAColor color = 0x00_rgba;
    color.a = alpha.value();
    gcx.drawFilledRect({0, 0, gcx.screenWidth(), gcx.screenHeight()}, color);
}

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
