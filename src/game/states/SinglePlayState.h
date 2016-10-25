#pragma once

#include "game/GameState.h"
#include "game/layout/singleplayer/LeftSidebarBox.h"
#include "game/layout/singleplayer/RightSidebarBox.h"
#include "game/layout/singleplayer/WellBox.h"

#include <map>
#include <stack>
#include <string>


class Texture;
class Music;
class SoundEffect;

enum class ScoreType : uint8_t {
    CLEAR_SINGLE,
    CLEAR_DOUBLE,
    CLEAR_TRIPLE,
    CLEAR_PERFECT,
    MINI_TSPIN,
    CLEAR_MINI_TSPIN_SINGLE,
    TSPIN,
    CLEAR_TSPIN_SINGLE,
    CLEAR_TSPIN_DOUBLE,
    CLEAR_TSPIN_TRIPLE,
    SOFTDROP,
    HARDDROP
};


class SinglePlayState: public GameState {
public:
    SinglePlayState(AppContext&);
    ~SinglePlayState();

    void update(const std::vector<InputEvent>&, AppContext&) final;
    void draw(GraphicsContext&) final;

private:
    bool paused;
    bool gameover;
    std::unique_ptr<Texture> tex_background;
    std::shared_ptr<Music> music;
    std::shared_ptr<SoundEffect> sfx_onhold;
    std::shared_ptr<SoundEffect> sfx_onlevelup;
    std::shared_ptr<SoundEffect> sfx_onlineclear;
    std::shared_ptr<SoundEffect> sfx_onlock;
    std::shared_ptr<SoundEffect> sfx_onrotate;
    bool texts_need_update;

    Layout::WellBox ui_well;
    Layout::LeftSidebarBox ui_leftside;
    Layout::RightSidebarBox ui_rightside;

    const unsigned lineclears_per_level;
    int lineclears_left;
    std::stack<Duration> gravity_levels;
    unsigned current_level;
    unsigned current_score;
    std::map<ScoreType, unsigned> score_table;
    ScoreType previous_lineclear_type;

    void registerObservers();
    void addNextPiece();
};
