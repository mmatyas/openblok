#pragma once

#include "game/layout/options/CategoryBtn.h"
#include "game/states/substates/MainMenu.h"
#include "system/Rectangle.h"

#include <functional>
#include <memory>

namespace Layout { namespace Options { class OptionsItem; } }


namespace SubStates {
namespace MainMenu {

class Options: public State {
public:
    Options(MainMenuState&, AppContext&);
    ~Options();
    void update(MainMenuState&, const std::vector<Event>&, AppContext&) final;
    void draw(MainMenuState&, GraphicsContext&) const final;

private:
    ::Rectangle screen_rect;
    ::Rectangle container_rect;

    std::vector<Layout::Options::CategoryButton> category_buttons;
    std::vector<std::vector<std::unique_ptr<Layout::Options::OptionsItem>>> subitem_panels;
    unsigned current_category_idx;
    unsigned current_setting_idx;

    std::function<void(InputType)> fn_category_input;
    std::function<void(InputType)> fn_settings_input;
    std::function<void(InputType)>* current_input_handler;

    void updatePositions(GraphicsContext&);
};

} // namespace MainMenu
} // namespace SubStates
