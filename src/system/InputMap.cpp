#include "InputMap.h"

EventToButtonsMap toEventMap(const ButtonToEventsMap& buttonmap)
{
    EventToButtonsMap eventmap;
    for (const auto& curr_map : buttonmap) { // pair <button, [events]>
        for (const auto& event : curr_map.second)
            eventmap[event].emplace_back(curr_map.first);
    }
    return eventmap;
}

ButtonToEventsMap toButtonMap(const EventToButtonsMap& eventmap)
{
    ButtonToEventsMap buttonmap;
    for (const auto& curr_map : eventmap) { // pair <event, [buttons]>
        for (const auto& button : curr_map.second)
            buttonmap[button].emplace_back(curr_map.first);
    }
    return buttonmap;
}
