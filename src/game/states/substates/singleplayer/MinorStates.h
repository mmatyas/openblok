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
    StateType type() const { return StateType::FADE_IN; }
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    Transition<uint8_t> alpha;
};

class FadeOut : public State {
public:
    FadeOut(AppContext&);
    StateType type() const { return StateType::FADE_OUT; }
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    Transition<uint8_t> alpha;
};

class Countdown : public State {
public:
    Countdown(AppContext& app);
    StateType type() const { return StateType::COUNTDOWN; }
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
    StateType type() const { return StateType::PAUSED; }
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    std::unique_ptr<Texture> tex;
};

class GameOver : public State {
public:
    GameOver(SinglePlayState&, AppContext&);
    StateType type() const { return StateType::GAME_OVER; }
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
    StateType type() const { return StateType::GAME_COMPLETE; }
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
