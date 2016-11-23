#pragma once

#include "game/Transition.h"
#include "game/layout/menu/Logo.h"
#include "game/layout/menu/MainMenuBtn.h"
#include "game/layout/menu/PieceRain.h"
#include "game/states/substates/MainMenu.h"

#include <array>
#include <memory>

class GameState;
class Music;
class SoundEffect;
class Texture;


namespace SubStates {
namespace MainMenu {

class Base : public State {
public:
    Base(MainMenuState&, AppContext&);
    ~Base();
    void updateAnimationsOnly(MainMenuState&, AppContext&) final;
    void update(MainMenuState&, const std::vector<Event>&, AppContext&) final;
    void draw(MainMenuState&, GraphicsContext&) const final;

private:
    std::unique_ptr<Texture> tex_background;
    Layout::Logo logo;
    std::array<Layout::PieceRain, 2> rains;

    std::vector<Layout::MainMenuButton> buttons;
    unsigned current_button_index;

    std::shared_ptr<Music> music;
    std::unique_ptr<Transition<uint8_t>> state_transition_alpha;

    void updatePositions(GraphicsContext&);
    void onFadeoutComplete(AppContext&, std::unique_ptr<GameState>&&);
};

} // namespace MainMenu
} // namespace SubStates
