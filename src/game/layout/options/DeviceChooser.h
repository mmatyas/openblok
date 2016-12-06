#pragma once

#include "ValueChooser.h"

using DeviceID = int8_t;


namespace Layout {
namespace Options {

class ValueChooser;

class DeviceChooser : public ValueChooser {
public:
    DeviceChooser(AppContext&,
                  std::string&& label,
                  std::function<void(DeviceID)>&& on_change = [](DeviceID){});
    ~DeviceChooser();

    void onPress(AppContext&, InputType) override;

private:
    std::vector<DeviceID> device_ids;
    size_t current_idx;

    void onLeftPress();
    void onRightPress();
    std::function<void(DeviceID)> callback;
};

} // namespace Options
} // namespace Layout
