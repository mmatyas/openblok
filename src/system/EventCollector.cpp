#include "EventCollector.h"

#include "sdl/SDLEventCollector.h"

std::unique_ptr<EventCollector> EventCollector::create()
{
    return std::make_unique<SDLEventCollector>();
}
