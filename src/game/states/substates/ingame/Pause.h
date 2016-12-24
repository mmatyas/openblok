#pragma once

#include "game/states/substates/Ingame.h"

#include <array>
#include <memory>

class Texture;


namespace SubStates {
namespace Ingame {
namespace States {

class Pause : public State {
public:
    Pause(AppContext&);
    void update(IngameState&, const std::vector<Event>&, AppContext&) final;
    void drawActive(IngameState&, GraphicsContext&) const final;

private:
    std::unique_ptr<Texture> tex_pause;
    std::vector<std::array<std::unique_ptr<Texture>, 2>> tex_menuitems;
    size_t current_menuitem;
};

} // namespace States
} // namespace Ingame
} // namespace SubStates
