#pragma once

#include "game/layout/options/OptionsItem.h"

#include <functional>
#include <vector>


namespace Layout {
namespace Options {

class ValueChooser : public OptionsItem {
public:
    ValueChooser(AppContext&,
                 std::vector<std::string>&& possible_values, size_t start_index,
                 std::string&& label, std::string&& description = "",
                 std::function<void(const std::string&)>&& on_change = [](const std::string&){});
    ~ValueChooser();

    void onPress(AppContext&, InputType) override;
    void draw(GraphicsContext&) const override;

    void setWidth(int) override;

private:
    const std::vector<std::string> values;
    std::vector<std::unique_ptr<Texture>> tex_actives;
    std::vector<std::unique_ptr<Texture>> tex_inactives;
    size_t current_idx;

    int right_x;
    int padding_ver;
    static constexpr int padding_hor = 20;

    void onLeftPress();
    void onRightPress();
    std::function<void(const std::string)> callback;
};

} // namespace Options
} // namespace Layout
