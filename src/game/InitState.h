#pragma once

#include "GameState.h"

class InitState: public GameState {
public:
    void update(const std::vector<InputEvent>&, AppContext&) final;
    void draw() final;
};
