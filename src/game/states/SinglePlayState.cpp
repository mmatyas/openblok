#include "SinglePlayState.h"

#include "game/Resources.h"


SinglePlayState::SinglePlayState(AppContext&)
    : paused(false)
{
    board.addPiece(Piece::Type::S);
}

void SinglePlayState::update(const std::vector<InputEvent>& inputs, AppContext& ctx)
{
    for (const auto& input : inputs) {
        if (input.type() == InputType::PAUSE && input.down()) {
            paused = !paused;
            return;
        }
    }

    if (paused)
        return;

    board.update(inputs, ctx);
}

void SinglePlayState::draw(GraphicsContext& gcx)
{
    for (unsigned x = 0; x < gcx.screenWidth(); x += gcx.textureWidth(TexID::GAMEPLAYBG))
        gcx.drawTexture(TexID::GAMEPLAYBG, x, 0);

    board.draw(gcx, gcx.screenWidth() / 2 - 5 * Mino::texture_size_px, 8);
}
