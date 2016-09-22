#pragma once

#include "system/InputEvent.h"
#include "system/GraphicsContext.h"

#include <chrono>
#include <vector>


class AppContext;

class GameState {
public:
    virtual ~GameState() {};

    virtual void update(const std::vector<InputEvent>&, AppContext&) = 0;
    virtual void draw(GraphicsContext& gcx) = 0;

    virtual void on_pause() {}
    virtual void on_resume() {}

    // the fixed time step of the game logic
    static constexpr std::chrono::steady_clock::duration frame_duration
         = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
             std::chrono::duration<int, std::ratio<1, 60>>(1));
};
