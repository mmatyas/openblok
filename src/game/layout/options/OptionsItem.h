#pragma once

#include "game/layout/MenuItem.h"
#include "system/Color.h"
#include "system/Event.h"

#include <memory>

class AppContext;
class Texture;


namespace Layout {
namespace Options {

class OptionsItem : public Layout::MenuItem {
public:
    OptionsItem(AppContext&, std::string&& label, std::string&& description);
    virtual ~OptionsItem();

    /// Call for recognized key presses.
    virtual void onPress(AppContext&, InputType) {}
    /// Call for raw, possibly unknown key presses.
    virtual void onRawPress(AppContext&, RawInputEvent) {}

    /// The option item is waiting for input, and its parent should not move the focus away.
    virtual bool isLocked() const { return false; }

    /// The description of the item. Can be NULL.
    const std::unique_ptr<Texture>& descriptionTex() { return tex_description; }

    virtual void setWidth(int w) { bounding_box.w = w; }

    int marginBottom() const { return margin_bottom; }
    void setMarginBottom(int px) { margin_bottom = px; }

    static RGBAColor marker_color;

protected:
    std::unique_ptr<Texture> tex_label;
    std::unique_ptr<Texture> tex_description;

private:
    int margin_bottom;
};

} // namespace Options
} // namespace Layout
