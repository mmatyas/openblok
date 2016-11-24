#pragma once

#include "game/layout/options/CategoryBtn.h"
#include "game/states/substates/MainMenu.h"
#include "system/Rectangle.h"

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
    unsigned category_btn_idx;

    void updatePositions(GraphicsContext&);
};

} // namespace MainMenu
} // namespace SubStates
