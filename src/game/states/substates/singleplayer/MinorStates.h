#pragma once

#include "game/Transition.h"
#include "game/states/substates/SinglePlayer.h"

#include <array>
#include <memory>

class SoundEffect;
class Texture;


namespace SubStates {
namespace SinglePlayer {
namespace States {

class FadeIn : public State {
public:
    FadeIn();
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    Transition<uint8_t> alpha;
};

class FadeOut : public State {
public:
    FadeOut(AppContext&);
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    Transition<uint8_t> alpha;
};

class Countdown : public State {
public:
    Countdown(AppContext& app);
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    uint8_t current_idx;
    Transition<void> timer;

    std::array<std::shared_ptr<SoundEffect>, 3> sfx_countdown;
    std::array<std::unique_ptr<Texture>, 3> tex_countdown;
    std::shared_ptr<SoundEffect> requested_sfx;
};

class Pause : public State {
public:
    Pause(AppContext&);
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    std::unique_ptr<Texture> tex_pause;
    std::vector<std::array<std::unique_ptr<Texture>, 2>> tex_menuitems;
    size_t current_menuitem;
};

class GameOver : public State {
public:
    GameOver(SinglePlayState&, AppContext&);
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    std::unique_ptr<Texture> tex_gameover;
    std::shared_ptr<SoundEffect> sfx_ongameover;
    Transition<double> background_percent;
    Transition<void> statistics_delay;
};

class GameComplete : public State {
public:
    GameComplete(SinglePlayState&, AppContext&);
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    std::unique_ptr<Texture> tex_finish;
    std::shared_ptr<SoundEffect> sfx_onfinish;
    Transition<uint8_t> text_alpha;
    Transition<void> statistics_delay;
};

} // namespace States
} // namespace SinglePlayer
} // namespace SubStates
