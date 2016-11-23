#pragma once

#include "game/Transition.h"
#include "game/states/substates/MainMenu.h"
#include "system/Rectangle.h"

#include <memory>


namespace SubStates {
namespace MainMenu {

class Options: public State {
public:
    Options(MainMenuState&, AppContext&);
    ~Options();
    void update(MainMenuState&, const std::vector<Event>&, AppContext&) final;
    void draw(MainMenuState&, GraphicsContext&) const final;

private:
    ::Rectangle container_rect;
    ::Rectangle border_rect;
    std::unique_ptr<Transition<uint8_t>> fade_alpha;

    void updatePositions(GraphicsContext&);
};

} // namespace MainMenu
} // namespace SubStates
