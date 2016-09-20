#include "SinglePlayState.h"

#include "game/Resources.h"
#include "game/WellEvent.h"


SinglePlayState::SinglePlayState(AppContext&)
    : paused(false)
    , next_pieces(4)
{
    board.registerObserver(WellEvent::NEXT_REQUESTED, [this](){
        this->board.addPiece(this->next_pieces.next());
        this->piece_holder.onNextTurn();
    });

    board.registerObserver(WellEvent::HOLD_REQUESTED, [this](){
        this->piece_holder.onSwapRequested();
        if (this->piece_holder.swapAllowed()) {
            auto type = this->board.activePiece()->type();
            this->board.deletePiece();
            if (this->piece_holder.isEmpty())
                this->piece_holder.swapWithEmpty(type);
            else
                this->board.addPiece(this->piece_holder.swapWith(type));
        }
    });
}

void SinglePlayState::update(const std::vector<InputEvent>& inputs, AppContext& ctx)
{
    for (const auto& input : inputs) {
        if (input.type() == InputType::GAME_PAUSE && input.down()) {
            paused = !paused;
            return;
        }
    }

    if (paused)
        return;

    board.update(inputs, ctx);
    piece_holder.update();
}

void SinglePlayState::draw(GraphicsContext& gcx)
{
    for (unsigned x = 0; x < gcx.screenWidth(); x += gcx.textureWidth(TexID::GAMEPLAYBG))
        gcx.drawTexture(TexID::GAMEPLAYBG, x, 0);

    next_pieces.draw(gcx, gcx.screenWidth() / 2 + Mino::texture_size_px + 5 * Mino::texture_size_px, 8);
    piece_holder.draw(gcx, gcx.screenWidth() / 2 - 11 * Mino::texture_size_px, 8);
    board.draw(gcx, gcx.screenWidth() / 2 - 5 * Mino::texture_size_px, 8);
}
