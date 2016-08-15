#pragma once

#include "InputEvent.h"

#include <memory>
#include <vector>


class EventCollector {
public:
    static std::unique_ptr<EventCollector> create();

    virtual std::vector<InputEvent> collect() = 0;
    virtual bool quit_requested() = 0;
};
