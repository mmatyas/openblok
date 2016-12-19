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

void FadeIn::drawActive(MultiplayerState&, GraphicsContext& gcx) const
{
    const float scale = 0.8;
    const float inverse_scale = 1.f / scale;

    RGBAColor color = 0x00_rgba;
    color.a = alpha.value();
    gcx.drawFilledRect({
        0, 0,
        static_cast<int>(gcx.screenWidth() * inverse_scale),
        static_cast<int>(gcx.screenHeight() * inverse_scale)},
        color);
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

void FadeOut::drawActive(MultiplayerState&, GraphicsContext& gcx) const
{
    const float scale = 0.8;
    const float inverse_scale = 1.f / scale;

    RGBAColor color = 0x00_rgba;
    color.a = alpha.value();
    gcx.drawFilledRect({
        0, 0,
        static_cast<int>(gcx.screenWidth() * inverse_scale),
        static_cast<int>(gcx.screenHeight() * inverse_scale)},
        color);
}

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
