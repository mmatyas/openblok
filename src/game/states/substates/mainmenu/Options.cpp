#include "Options.h"

#include "game/AppContext.h"
#include "game/states/MainMenuState.h"
#include "system/GraphicsContext.h"


namespace SubStates {
namespace MainMenu {

Options::Options(MainMenuState&, AppContext& app)
    : fade_alpha(std::make_unique<Transition<uint8_t>>(
        std::chrono::milliseconds(500),
        [](double t){ return t * 0xFF; }
    ))
{
    updatePositions(app.gcx());
}

Options::~Options()
{
}

void Options::updatePositions(GraphicsContext& gcx)
{
    int margin_x = gcx.screenWidth() * 0.1;
    int margin_y = gcx.screenHeight() * 0.1;
    container_rect = {
        margin_x, margin_y,
        gcx.screenWidth() - margin_x * 2, gcx.screenHeight() - margin_y * 2
    };
    static const int border_width = 4;
    border_rect = {
        container_rect.x - border_width, container_rect.y - border_width,
        container_rect.w + border_width * 2, container_rect.h + border_width * 2
    };
}

void Options::update(MainMenuState& parent, const std::vector<Event>& events, AppContext& app)
{
    parent.states.front()->updateAnimationsOnly(parent, app);

    for (const auto& event : events) {
        if (event.type == EventType::INPUT && event.input.type() == InputType::MENU_CANCEL) {
            assert(parent.states.size() > 1);
            fade_alpha = std::make_unique<Transition<uint8_t>>(
                std::chrono::milliseconds(500),
                [](double t){ return (1.0 - t) * 0xFF; },
                [&parent](){ parent.states.pop_back(); });
        }
    }

    fade_alpha->update(Timing::frame_duration);
}

void Options::draw(MainMenuState& parent, GraphicsContext& gcx) const
{
    parent.states.front()->draw(parent, gcx);

    RGBAColor color = 0x0000FFFF_rgba;
    color.a = fade_alpha->value();
    gcx.drawFilledRect(border_rect, color);
    color = 0x2030FFFF_rgba;
    color.a = fade_alpha->value();
    gcx.drawFilledRect(container_rect, color);
}

} // namespace MainMenu
} // namespace SubStates
