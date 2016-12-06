#pragma once

#include "game/layout/options/OptionsItem.h"

#include <functional>

class Font;
using DeviceID = int8_t;


namespace Layout {
namespace Options {

class InputField : public OptionsItem {
public:
    InputField(AppContext&, std::string&& label, DeviceID, uint16_t default_raw_key,
               std::function<void(uint16_t)>&& on_change = [](uint16_t){});

    void onPress(AppContext&, InputType) override;
    void onRawPress(AppContext&, uint16_t) override;

    bool isLocked() const override { return waiting_for_input; }
    void draw(GraphicsContext&) const override;
    void setWidth(int) override;

private:
    const DeviceID device_id;
    bool waiting_for_input;

    std::shared_ptr<Font> font;
    std::unique_ptr<Texture> tex_buttonname;

    int right_x;
    int padding_ver;
    static constexpr int padding_hor = 20;

    void changeButtonTex(AppContext&, uint16_t);
    std::function<void(uint16_t)> callback;
};

} // namespace Options
} // namespace Layout
