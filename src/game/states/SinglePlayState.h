#pragma once

#include "game/GameState.h"
#include "game/components/HoldQueue.h"
#include "game/components/NextQueue.h"
#include "game/components/Well.h"


class SinglePlayState: public GameState {
public:
    SinglePlayState(AppContext&);
    void update(const std::vector<InputEvent>&, AppContext&) final;
    void draw(GraphicsContext&) final;

private:
    bool paused;
    TextureID tex_background;
    TextureID tex_hold;
    TextureID tex_next;
    TextureID tex_goal;

    HoldQueue piece_holder;
    NextQueue next_pieces;
    Well board;

    void addNextPiece();
};
