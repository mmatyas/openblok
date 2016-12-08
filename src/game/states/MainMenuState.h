#pragma once

#include "game/GameState.h"
#include "game/Transition.h"

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

private:
    Transition<void> padconnect_phase1; ///< phase 1: the popup is visible for a few seconds
    Transition<uint8_t> padconnect_phase2; ///< phase 2: the popup disappears in a few seconds
    std::unique_ptr<Texture> tex_padconnected;
};
