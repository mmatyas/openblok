#pragma once

#include "system/Event.h"
#include "system/GraphicsContext.h"

#include <chrono>
#include <vector>


class AppContext;

class GameState {
public:
    virtual ~GameState() {};

    virtual void update(const std::vector<Event>&, AppContext&) = 0;
    virtual void draw(GraphicsContext& gcx) = 0;

    virtual void on_pause() {}
    virtual void on_resume() {}
};
