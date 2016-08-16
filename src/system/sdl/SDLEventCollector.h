#pragma once

#include "system/EventCollector.h"

class SDLEventCollector: public EventCollector {
public:
    SDLEventCollector();

    std::vector<InputEvent> collect() final;
    bool quit_requested() final { return m_quit_requested; }

private:
    bool m_quit_requested;
};
