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

    HoldQueue piece_holder;
    NextQueue next_pieces;
    Well board;
};
