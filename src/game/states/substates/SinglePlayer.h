#pragma once

#include "game/ScoreTable.h"
#include "game/Transition.h"
#include "system/Event.h"

#include <array>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>


class AppContext;
class GraphicsContext;
class Font;
class Music;
class SinglePlayState;
class SoundEffect;
class TextPopup;
class Texture;


namespace SubStates {
namespace SinglePlayer {

enum class StateType : uint8_t {
    FADE_IN,
    COUNTDOWN,
    GAME_RUNNING,
    PAUSED,
    GAME_OVER,
    STATISTICS,
};

class State {
public:
    virtual ~State() {}
    virtual StateType type() const = 0;
    virtual void update(SinglePlayState&, const std::vector<Event>&, AppContext&) = 0;
    virtual void draw(SinglePlayState&, GraphicsContext&) const {};
};

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

class Statistics : public State {
public:
    Statistics(SinglePlayState&, AppContext&);
    StateType type() const { return StateType::STATISTICS; }
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    std::unique_ptr<Texture> tex_title;
    std::vector<std::pair<std::unique_ptr<Texture>, std::unique_ptr<Texture>>> score_texs;
    Transition<double> background_percent;
    Transition<uint8_t> title_alpha;
    Transition<uint8_t> displayed_item_count;
    const int column_width;
    const int column_padding;
    const int title_padding_bottom;
};

class Gameplay : public State {
public:
    Gameplay(SinglePlayState&, AppContext&);
    StateType type() const { return StateType::GAME_RUNNING; }
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    std::shared_ptr<Music> music;
    std::shared_ptr<SoundEffect> sfx_onhold;
    std::shared_ptr<SoundEffect> sfx_onlevelup;
    std::array<std::shared_ptr<SoundEffect>, 4> sfx_onlineclear;
    std::shared_ptr<SoundEffect> sfx_onlock;
    std::shared_ptr<SoundEffect> sfx_onrotate;
    std::shared_ptr<Font> font_popuptext;
    bool texts_need_update;

    Transition<void> pending_levelup_msg;
    std::vector<std::unique_ptr<TextPopup>> textpopups;

    const unsigned lineclears_per_level;
    int lineclears_left;
    std::stack<Duration> gravity_levels;
    ScoreType previous_lineclear_type;
    unsigned short back2back_length;

    void addNextPiece(SinglePlayState& parent);
    void registerObservers(SinglePlayState& parent, AppContext&);
};

} // namespace States
} // namespace SinglePlayer
} // namespace SubStates
