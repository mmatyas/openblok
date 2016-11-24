#include "Options.h"

#include "game/AppContext.h"
#include "game/states/MainMenuState.h"
#include "system/GraphicsContext.h"

#include <assert.h>


namespace SubStates {
namespace MainMenu {

Options::Options(MainMenuState&, AppContext& app)
{
    updatePositions(app.gcx());
}

Options::~Options()
{
}

void Options::updatePositions(GraphicsContext& gcx)
{
    const int width = gcx.screenWidth() * 0.90;
    const int height = 600; // TODO: fix magic numbers
    container_rect = {
        (gcx.screenWidth() - width) / 2, (gcx.screenHeight() - height) / 2,
        width, height
    };
    screen_rect = {0, 0, gcx.screenWidth(), gcx.screenHeight()};
}

void Options::update(MainMenuState& parent, const std::vector<Event>& events, AppContext& app)
{
    parent.states.front()->updateAnimationsOnly(parent, app);

    for (const auto& event : events) {
        if (event.type == EventType::INPUT && event.input.type() == InputType::MENU_CANCEL) {
            assert(parent.states.size() > 1);
            parent.states.pop_back();
        }
    }
}

void Options::draw(MainMenuState& parent, GraphicsContext& gcx) const
{
    parent.states.front()->draw(parent, gcx);

    RGBAColor black = 0x00000080_rgba;
    gcx.drawFilledRect(screen_rect, black);
    RGBAColor panel_bg = 0x002060F0_rgba;
    gcx.drawFilledRect(container_rect, panel_bg);
}

} // namespace MainMenu
} // namespace SubStates
