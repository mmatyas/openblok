#pragma once

#include "SysConfig.h"
#include "Theme.h"
#include "WellConfig.h"
#include "system/InputConfigFile.h"
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
    InputConfigFile& inputconfig() { return m_inputconfig; }
    SysConfig& sysconfig() { return m_sysconfig; }
    ThemeConfig& theme() { return m_themeconfig; }
    WellConfig& wellconfig() { return m_wellconfig; }
    std::stack<std::unique_ptr<GameState>>& states() { return m_states; }

private:
    std::unique_ptr<Window> m_window;
    InputConfigFile m_inputconfig;
    SysConfig m_sysconfig;
    ThemeConfig m_themeconfig;
    WellConfig m_wellconfig;
    std::stack<std::unique_ptr<GameState>> m_states;
};
