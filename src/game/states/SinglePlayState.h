#pragma once

#include "game/GameState.h"
#include "game/components/singleplayer/LeftSidebarBox.h"
#include "game/components/singleplayer/RightSidebarBox.h"
#include "game/components/singleplayer/WellBox.h"

#include <stack>
#include <string>


enum ScoreTypes {
    LINE_CLEAR_SINGLE = 1,
    LINE_CLEAR_DOUBLE = 2,
    LINE_CLEAR_TRIPLE = 3,
    LINE_CLEAR_PERFECT = 4,
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
    TextureID tex_background;
    bool texts_need_update;

    Layout::WellBox ui_well;
    Layout::LeftSidebarBox ui_leftside;
    Layout::RightSidebarBox ui_rightside;

    const unsigned lineclears_per_level;
    int lineclears_left;
    std::stack<Duration> gravity_levels;
    unsigned current_level;
    unsigned current_score;
    std::unordered_map<ScoreTypes, unsigned, std::hash<size_t>> score_table;
    ScoreTypes previous_lineclear_type;

    void registerObservers();
    void addNextPiece();
};
