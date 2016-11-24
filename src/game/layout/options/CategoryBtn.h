#pragma once

#include "game/layout/Button.h"

#include <memory>


class AppContext;
class Texture;


namespace Layout {
class OptionsCategoryButton : public Button {
public:
    OptionsCategoryButton(AppContext&, std::string&& label, std::function<void()>&& on_press);

    void setPosition(int x, int y) override;

    void update() override;
    void draw(GraphicsContext&) const override;

private:
    std::unique_ptr<Texture> tex_label;
};
} // namespace Layout
