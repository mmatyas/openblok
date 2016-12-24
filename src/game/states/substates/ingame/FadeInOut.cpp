#include "FadeInOut.h"

#include "system/GraphicsContext.h"


namespace SubStates {
namespace Ingame {
namespace States {

FadeIn::FadeIn(std::function<void()>&& on_end, float scale)
    : scale(scale)
    , alpha(std::chrono::milliseconds(300),
            [](double t){ return (1.0 - t) * 0xFF; },
            std::move(on_end))
{}

void FadeIn::update(IngameState&, const std::vector<Event>&, AppContext&)
{
    alpha.update(Timing::frame_duration);
}

void FadeIn::drawActive(IngameState&, GraphicsContext& gcx) const
{
    RGBAColor color = 0x00_rgba;
    color.a = alpha.value();
    gcx.drawFilledRect({
        0, 0,
        static_cast<int>(gcx.screenWidth() * scale),
        static_cast<int>(gcx.screenHeight() * scale)},
        color);
}

FadeOut::FadeOut(std::function<void()>&& on_end, float scale)
    : scale(scale)
    , alpha(std::chrono::milliseconds(300),
            [](double t){ return t * 0xFF; },
            std::move(on_end))
{}

void FadeOut::update(IngameState&, const std::vector<Event>&, AppContext&)
{
    alpha.update(Timing::frame_duration);
}

void FadeOut::drawActive(IngameState&, GraphicsContext& gcx) const
{
    RGBAColor color = 0x00_rgba;
    color.a = alpha.value();
    gcx.drawFilledRect({
        0, 0,
        static_cast<int>(gcx.screenWidth() * scale),
        static_cast<int>(gcx.screenHeight() * scale)},
        color);
}

} // namespace States
} // namespace Ingame
} // namespace SubStates
