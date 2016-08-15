#pragma once

#include "system/InputEvent.h"

#include <vector>


class AppContext;

class GameState {
public:
    virtual void update(const std::vector<InputEvent>&, AppContext&) = 0;
    virtual void draw() = 0;

    virtual void on_pause() {}
    virtual void on_resume() {}
};
