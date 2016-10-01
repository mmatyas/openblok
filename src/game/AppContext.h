#pragma once

#include "game/GameState.h"
#include "system/FontManager.h"
#include "system/Window.h"

#include <memory>
#include <stack>


class AppContext {
public:
    bool init();

    Window& window() { return *m_window; }
    GraphicsContext& gcx() { return m_window->graphicsContext(); }
    FontManager& fontman() { return *m_fontman; }
    std::stack<std::unique_ptr<GameState>>& states() { return m_states; }

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<FontManager> m_fontman;
    std::stack<std::unique_ptr<GameState>> m_states;
};
