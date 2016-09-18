#pragma once

#include "game/GameState.h"
#include "game/components/Holder.h"
#include "game/components/PieceQueue.h"
#include "game/components/Well.h"


class SinglePlayState: public GameState {
public:
    SinglePlayState(AppContext&);
    void update(const std::vector<InputEvent>&, AppContext&) final;
    void draw(GraphicsContext&) final;

private:
    bool paused;
    Holder piece_holder;
    PieceQueue next_pieces;
    Well board;
};
