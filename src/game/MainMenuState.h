#pragma once

#include "GameState.h"


class MainMenuState: public GameState {
public:
    MainMenuState(AppContext&);
    void update(const std::vector<InputEvent>&, AppContext&) final;
    void draw(GraphicsContext& gcx) final;
};
