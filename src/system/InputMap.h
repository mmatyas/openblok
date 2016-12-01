#pragma once

#include "Event.h"

#include <stdint.h>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>


using DeviceName = const std::string;
using EventToButtonsMap = std::unordered_map<InputType, std::vector<uint16_t>, InputTypeHash>;
using ButtonToEventsMap = std::unordered_map<uint16_t, std::vector<InputType>>;

enum class DeviceType : uint8_t {
    KEYBOARD,
    GAMEPAD,
    LEGACY_JOYSTICK,
};

struct DeviceData {
    DeviceType type;
    EventToButtonsMap eventmap;
};

using DeviceMap = std::map<DeviceName, DeviceData>;
