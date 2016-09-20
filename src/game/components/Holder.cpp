#include "Holder.h"

#include "PieceFactory.h"
#include "game/GameState.h"
#include "system/GraphicsContext.h"

#include <assert.h>


Holder::Holder()
    : swap_allowed(true)
    , empty(true)
    , current_piece(Piece::allTypes.at(0))
    , swapblocked_alpha(std::chrono::milliseconds(500), [](double t){
            return static_cast<uint8_t>((1.0 - t) * 0xFF);
        },
        [this](){ this->swapblocked_alpha.stop(); })
{
    swapblocked_alpha.stop();

    size_t i = 0;
    for(const auto p : Piece::allTypes) {
        piece_storage[i] = PieceFactory::make_uptr(p);
        i++;
    }
}

void Holder::onSwapRequested()
{
    if (!swap_allowed)
        swapblocked_alpha.unpause();
}

void Holder::onNextTurn()
{
    swap_allowed = true;
}

Piece::Type Holder::swapWith(Piece::Type other)
{
    // TODO: use optional
    assert(!empty);

    auto prev = current_piece;
    current_piece = other;

    swap_allowed = false;
    empty = false;
    swapblocked_alpha.stop();

    return prev;
}

void Holder::swapWithEmpty(Piece::Type other)
{
    // TODO: use optional
    assert(empty);

    current_piece = other;
    swap_allowed = false;
    swapblocked_alpha.stop();

    empty = false;
}

void Holder::update()
{
    swapblocked_alpha.update(GameState::frame_duration);
}

void Holder::draw(GraphicsContext& gcx, int x, int y)
{
    gcx.drawFilledRect({x, y, 5 * Mino::texture_size_px, 4 * Mino::texture_size_px}, 0x0A0AFF_rgb);

    if (swapblocked_alpha.running()) {
        gcx.drawFilledRect({
            x, y + 4 * Mino::texture_size_px,
            5 * Mino::texture_size_px, 10},
            {0xFF, 0x0, 0x0, swapblocked_alpha.value()});
    }

    if (!empty) {
        piece_storage.at(static_cast<size_t>(current_piece))->draw(gcx,
                                                                   x + Mino::texture_size_px / 2,
                                                                   y + Mino::texture_size_px);
    }
}
