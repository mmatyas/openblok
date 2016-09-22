#pragma once

#include "game/GameState.h"
#include "system/EventCollector.h"
#include "system/FontManager.h"
#include "system/GraphicsContext.h"

#include <memory>
#include <stack>


class AppContext {
public:
    bool init();

    std::unique_ptr<GraphicsContext> gcx;
    std::unique_ptr<EventCollector> events;
    std::unique_ptr<FontManager> fonts;
    std::stack<std::unique_ptr<GameState>> states;
};
