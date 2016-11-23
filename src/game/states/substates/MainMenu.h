#pragma once

#include "system/Event.h"

#include <vector>

class AppContext;
class GraphicsContext;
class MainMenuState;


namespace SubStates {
namespace MainMenu {

class State {
public:
    virtual ~State() {}
    virtual void updateAnimationsOnly(MainMenuState&, AppContext&) {}
    virtual void update(MainMenuState&, const std::vector<Event>&, AppContext&) = 0;
    virtual void draw(MainMenuState&, GraphicsContext&) const {}
};

} // namespace MainMenu
} // namespace SubStates
