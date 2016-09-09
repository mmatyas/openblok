#include "SinglePlayState.h"

#include "game/Resources.h"


SinglePlayState::SinglePlayState(AppContext&)
{
    board.addPiece(Piece::Type::S);
}

void SinglePlayState::update(const std::vector<InputEvent>&, AppContext&)
{
}

void SinglePlayState::draw(GraphicsContext& gcx)
{
    for (unsigned x = 0; x < gcx.screenWidth(); x += gcx.textureWidth(TexID::GAMEPLAYBG))
        gcx.drawTexture(TexID::GAMEPLAYBG, x, 0);

    board.draw(gcx, gcx.screenWidth() / 2 - 5 * Mino::texture_size_px, 8);
}
