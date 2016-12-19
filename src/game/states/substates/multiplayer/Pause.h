#pragma once

#include "game/states/substates/Multiplayer.h"

#include <array>
#include <memory>

class Texture;


namespace SubStates {
namespace Multiplayer {
namespace States {

class Pause : public State {
public:
    Pause(AppContext&);
    void update(MultiplayerState&, const std::vector<Event>&, AppContext&) final;
    void drawActive(MultiplayerState&, GraphicsContext&) const final;

private:
    std::unique_ptr<Texture> tex_pause;
    std::vector<std::array<std::unique_ptr<Texture>, 2>> tex_menuitems;
    size_t current_menuitem;
};

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
