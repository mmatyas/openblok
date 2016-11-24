#include "Options.h"

#include "game/AppContext.h"
#include "game/layout/options/OptionsItem.h"
#include "game/layout/options/ToggleBtn.h"
#include "game/states/MainMenuState.h"
#include "game/util/CircularModulo.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Texture.h"

#include <assert.h>


namespace SubStates {
namespace MainMenu {

Options::Options(MainMenuState& parent, AppContext& app)
    : current_category_idx(0)
    , current_setting_idx(0)
{
    category_buttons.emplace_back(app, tr("SYSTEM"));
    category_buttons.emplace_back(app, tr("FINE TUNING"));
    category_buttons.emplace_back(app, tr("INPUT"));
    category_buttons.at(current_category_idx).onHoverEnter();

    using ToggleButton = Layout::Options::ToggleButton;

    std::vector<std::unique_ptr<Layout::Options::OptionsItem>> system_options;
    system_options.emplace_back(std::make_unique<ToggleButton>(app, false, tr("Fullscreen mode")));
    system_options.back()->setMarginBottom(40);
    system_options.emplace_back(std::make_unique<ToggleButton>(app, true, tr("Sound effects")));
    system_options.emplace_back(std::make_unique<ToggleButton>(app, true, tr("Background music")));
    subitem_panels.push_back(std::move(system_options));

    updatePositions(app.gcx());

    fn_category_input = [this, &parent](InputType input){
        switch (input) {
            case InputType::MENU_OK:
                current_input_handler = &fn_settings_input;
                subitem_panels.at(current_category_idx).at(current_setting_idx)->onHoverEnter();
                break;
            case InputType::MENU_CANCEL:
                assert(parent.states.size() > 1);
                parent.states.pop_back();
                break;
            case InputType::MENU_UP:
                category_buttons.at(current_category_idx).onHoverLeave();
                current_category_idx = circularModulo(
                    static_cast<int>(current_category_idx) - 1, category_buttons.size());
                category_buttons.at(current_category_idx).onHoverEnter();
                break;
            case InputType::MENU_DOWN:
                category_buttons.at(current_category_idx).onHoverLeave();
                current_category_idx = circularModulo(
                    static_cast<int>(current_category_idx) + 1, category_buttons.size());
                category_buttons.at(current_category_idx).onHoverEnter();
                break;
            default:
                break;
        }
    };
    fn_settings_input = [this](InputType input){
        auto& panel = subitem_panels.at(current_category_idx);
        switch (input) {
            case InputType::MENU_CANCEL:
                panel.at(current_setting_idx)->onHoverLeave();
                current_setting_idx = 0;
                current_input_handler = &fn_category_input;
                break;
            case InputType::MENU_UP:
                panel.at(current_setting_idx)->onHoverLeave();
                current_setting_idx = circularModulo(
                    static_cast<int>(current_setting_idx) - 1, category_buttons.size());
                panel.at(current_setting_idx)->onHoverEnter();
                break;
            case InputType::MENU_DOWN:
                panel.at(current_setting_idx)->onHoverLeave();
                current_setting_idx = circularModulo(
                    static_cast<int>(current_setting_idx) + 1, category_buttons.size());
                panel.at(current_setting_idx)->onHoverEnter();
                break;
            case InputType::MENU_LEFT:
                panel.at(current_setting_idx)->onLeftPress();
                break;
            case InputType::MENU_RIGHT:
                panel.at(current_setting_idx)->onRightPress();
                break;
            default:
                break;
        }
    };
    current_input_handler = &fn_category_input;
}

Options::~Options() = default;

void Options::updatePositions(GraphicsContext& gcx)
{
    const int width = gcx.screenWidth() * 0.90;
    const int height = 600; // TODO: fix magic numbers
    container_rect = {
        (gcx.screenWidth() - width) / 2, (gcx.screenHeight() - height) / 2,
        width, height
    };
    screen_rect = {0, 0, gcx.screenWidth(), gcx.screenHeight()};

    const int inner_x = container_rect.x + 12;
    category_buttons.at(0).setPosition(inner_x, container_rect.y + 12);
    for (unsigned i = 1; i < category_buttons.size(); i++) {
        const auto& prev = category_buttons.at(i - 1);
        category_buttons.at(i).setPosition(inner_x, prev.y() + prev.height() + 6);
    }

    const int category_column_width = category_buttons.at(0).width() + 12;
    const int subpanel_x = container_rect.x + category_column_width + 30;
    const int subpanel_item_width = container_rect.w - category_column_width - 30 * 2;
    for (auto& subpanel : subitem_panels) {
        subpanel.at(0)->setPosition(subpanel_x, container_rect.y + 30);
        subpanel.at(0)->setWidth(subpanel_item_width);
        for (unsigned i = 1; i < subpanel.size(); i++) {
            const auto& prev = subpanel.at(i - 1);
            subpanel.at(i)->setPosition(subpanel_x, prev->y() + prev->height() + prev->marginBottom());
            subpanel.at(i)->setWidth(subpanel_item_width);
        }
    }
}

void Options::update(MainMenuState& parent, const std::vector<Event>& events, AppContext& app)
{
    parent.states.front()->updateAnimationsOnly(parent, app);

    for (const auto& event : events) {
        switch (event.type) {
            case EventType::INPUT:
                if (!event.input.down())
                    continue;
                (*current_input_handler)(event.input.type());
                break;
            default:
                break;
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

    for (const auto& btn : category_buttons)
        btn.draw(gcx);

    assert(current_category_idx < subitem_panels.size());
    for (const auto& btn : subitem_panels.at(current_category_idx))
        btn->draw(gcx);
}

} // namespace MainMenu
} // namespace SubStates
