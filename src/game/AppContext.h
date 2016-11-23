#pragma once

#include "system/ConfigManager.h"
#include "system/Window.h"

#include <memory>
#include <stack>

class AudioContext;
class GameState;


class AppContext {
public:
    bool init();

    Window& window() { return *m_window; }
    GraphicsContext& gcx() { return m_window->graphicsContext(); }
    AudioContext& audio() { return m_window->audioContext(); }
    ConfigManager& config() { return m_config; }
    std::stack<std::unique_ptr<GameState>>& states() { return m_states; }

private:
    std::unique_ptr<Window> m_window;
    ConfigManager m_config;
    std::stack<std::unique_ptr<GameState>> m_states;
};
