#include "HoldQueue.h"

#include "Mino.h"
#include "PieceFactory.h"
#include "game/Timing.h"
#include "system/GraphicsContext.h"

#include <assert.h>


HoldQueue::HoldQueue()
    : swap_allowed(true)
    , empty(true)
    , current_piece(PieceTypeList.at(0))
    , swapblocked_alpha(std::chrono::milliseconds(500), [](double t){
            return static_cast<uint8_t>((1.0 - t) * 0xFF);
        },
        [this](){ this->swapblocked_alpha.stop(); })
{
    swapblocked_alpha.stop();

    size_t i = 0;
    for(const auto ptype : PieceTypeList) {
        piece_storage[i] = PieceFactory::make_uptr(ptype);
        i++;
    }
}

HoldQueue::~HoldQueue() = default;

void HoldQueue::onSwapRequested()
{
    if (!swap_allowed)
        swapblocked_alpha.unpause();
}

void HoldQueue::onNextTurn()
{
    swap_allowed = true;
}

PieceType HoldQueue::swapWith(PieceType other)
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

void HoldQueue::swapWithEmpty(PieceType other)
{
    // TODO: use optional
    assert(empty);

    current_piece = other;
    swap_allowed = false;
    swapblocked_alpha.stop();

    empty = false;
}

void HoldQueue::update()
{
    swapblocked_alpha.update(Timing::frame_duration);
}

void HoldQueue::draw(GraphicsContext& gcx, int x, int y, bool draw_panel) const
{
    if (draw_panel) {
        gcx.drawFilledRect({x, y,
            5 * Mino::texture_size_px, 4 * Mino::texture_size_px},
            0x0A0AFF80_rgba);
    }

    if (swapblocked_alpha.running()) {
        gcx.drawFilledRect({
            x, static_cast<short>(y + 4 * Mino::texture_size_px),
            static_cast<short>(5 * Mino::texture_size_px), 10},
            {0xFF, 0x0, 0x0, swapblocked_alpha.value()});
    }

    if (!empty) {
        const auto& piece = piece_storage.at(static_cast<size_t>(current_piece));
        const float padding_x = (4 - Piece::displayWidth(piece->type())) / 2.0f;
        piece->draw(x + Mino::texture_size_px * (0.5f + padding_x),
                    y + Mino::texture_size_px);
    }
}
