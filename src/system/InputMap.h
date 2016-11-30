#pragma once

#include "Event.h"

#include <stdint.h>
#include <map>
#include <string>
#include <vector>


enum class DeviceType : uint8_t {
    KEYBOARD,
    GAMEPAD,
    LEGACY_JOYSTICK,
};

using DeviceName = const std::string;
using EventToButtonsMap = std::map<InputType, std::vector<uint16_t>>;
using ButtonToEventsMap = std::map<uint16_t, std::vector<InputType>>;
using DeviceMaps = std::map<DeviceName, std::pair<DeviceType, EventToButtonsMap>>;
