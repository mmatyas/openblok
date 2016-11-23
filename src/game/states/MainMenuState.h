#pragma once

#include "game/GameState.h"

#include <list>
#include <memory>

namespace SubStates { namespace MainMenu { class State; } }


class MainMenuState: public GameState {
public:
    MainMenuState(AppContext&);
    ~MainMenuState();
    void update(const std::vector<Event>&, AppContext&) final;
    void draw(GraphicsContext&) final;

    std::list<std::unique_ptr<SubStates::MainMenu::State>> states;
};
