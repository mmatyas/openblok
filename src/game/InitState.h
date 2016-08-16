#pragma once

#include "GameState.h"

class InitState: public GameState {
public:
    InitState(AppContext&);
    void update(const std::vector<InputEvent>&, AppContext&) final;
    void draw(GraphicsContext& gcx) final;
};
