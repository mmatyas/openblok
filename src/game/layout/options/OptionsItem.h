#pragma once

#include "game/layout/Button.h"

#include <memory>

class AppContext;
class Texture;


namespace Layout {
namespace Options {

class OptionsItem : public Layout::Button {
public:
    OptionsItem(AppContext&, std::string&& label, std::function<void()>&& on_press = [](){});
    virtual ~OptionsItem();

    void update() override {}
    void setPosition(int x, int y) override;

    virtual void setWidth(int w) { bounding_box.w = w; }

    int marginBottom() const { return margin_bottom; }
    void setMarginBottom(int px) { margin_bottom = px; }

protected:
    std::unique_ptr<Texture> tex_label;

private:
    int margin_bottom;
};

} // namespace Options
} // namespace Layout
