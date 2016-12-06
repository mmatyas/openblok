#include "DeviceChooser.h"

#include "ValueChooser.h"
#include "game/AppContext.h"


std::vector<std::string> deviceNames(const DeviceMap& devices)
{
    std::vector<std::string> device_names;
    for (const auto& device : devices)
        device_names.emplace_back(device.second.name);
    return device_names;
}

namespace Layout {
namespace Options {

DeviceChooser::DeviceChooser(AppContext& app, std::string&& label,
                             std::function<void(DeviceID)>&& on_change)
    : ValueChooser(app, deviceNames(app.window().connectedDevices()), 0, std::forward<std::string>(label))
    , current_idx(0)
    , callback(on_change)
{
    const auto& devices = app.window().connectedDevices();
    for (const auto& device : devices)
        device_ids.emplace_back(device.second.id);
}

DeviceChooser::~DeviceChooser() = default;

void DeviceChooser::onPress(AppContext& app, InputType input)
{
    ValueChooser::onPress(app, input);
    switch (input) {
        case InputType::MENU_LEFT: onLeftPress(); break;
        case InputType::MENU_RIGHT: onRightPress(); break;
        default: break;
    }
}

void DeviceChooser::onLeftPress()
{
    if (current_idx > 0) {
        current_idx--;
        callback(device_ids.at(current_idx));
    }
}

void DeviceChooser::onRightPress()
{
    if (current_idx + 1 < device_ids.size()) {
        current_idx++;
        callback(device_ids.at(current_idx));
    }
}

} // namespace Options
} // namespace Layout
