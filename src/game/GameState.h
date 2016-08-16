#pragma once

#include "system/InputEvent.h"
#include "system/GraphicsContext.h"

#include <vector>


class AppContext;

class GameState {
public:
    virtual void update(const std::vector<InputEvent>&, AppContext&) = 0;
    virtual void draw(GraphicsContext& gcx) = 0;

    virtual void on_pause() {}
    virtual void on_resume() {}
};
