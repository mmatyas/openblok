#pragma once

#include "Button.h"

#include <memory>


class AppContext;
class Texture;


namespace Layout {
class MainMenuButton : public Button {
public:
    MainMenuButton(AppContext&, std::string&& label, std::function<void()>&& on_press);

    void update() override;
    void draw(GraphicsContext&) const override;

private:
    std::unique_ptr<Texture> tex_label_off;
    std::unique_ptr<Texture> tex_label_on;
};
} // namespace Layout
