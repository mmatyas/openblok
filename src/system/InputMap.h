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

using DeviceID = const std::string;
using ButtonMap = std::map<InputType, std::vector<uint16_t>>;
using Devices = std::map<DeviceID, std::pair<DeviceType, ButtonMap>>;
