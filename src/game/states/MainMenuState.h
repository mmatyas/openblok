#pragma once

#include "game/GameState.h"
#include "game/layout/menu/MainMenuBtn.h"


class Texture;
class Music;
class SoundEffect;


class MainMenuState: public GameState {
public:
    MainMenuState(AppContext&);
    ~MainMenuState();
    void update(const std::vector<InputEvent>&, AppContext&) final;
    void draw(GraphicsContext& gcx) final;

private:
    std::unique_ptr<Texture> tex_background;
    std::unique_ptr<Texture> tex_logo;
    std::vector<Layout::MainMenuButton> buttons;
    unsigned current_button_index;
};
