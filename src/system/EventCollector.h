#pragma once

#include "InputEvent.h"

#include <memory>
#include <vector>


class EventCollector {
public:
    virtual std::vector<InputEvent> collect() = 0;
    virtual bool quit_requested() = 0;

private:
    static std::unique_ptr<EventCollector> create();

friend class AppContext;
};
