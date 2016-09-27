#pragma once

#include "game/GameState.h"
#include "game/components/HoldQueue.h"
#include "game/components/NextQueue.h"
#include "game/components/Well.h"

#include <stack>


class SinglePlayState: public GameState {
public:
    SinglePlayState(AppContext&);
    void update(const std::vector<InputEvent>&, AppContext&) final;
    void draw(GraphicsContext&) final;

private:
    bool paused;
    FontID font_boxtitle;
    FontID font_boxcontent;
    TextureID tex_background;
    TextureID tex_hold;
    TextureID tex_next;
    TextureID tex_goal;
    TextureID tex_goal_counter;
    TextureID tex_level;
    TextureID tex_level_counter;
    bool texts_need_update;

    HoldQueue piece_holder;
    NextQueue next_pieces;
    Well board;

    const unsigned lineclears_per_level;
    int lineclears_left;
    std::stack<uint8_t> gravity_levels;
    unsigned current_level;

    void addNextPiece();
};
