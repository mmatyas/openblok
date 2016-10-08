#include "GameplayResources.h"

#include "game/components/MinoFactory.h"
#include "system/GraphicsContext.h"


namespace GameplayResources {

TextureID Textures::MINO_I = -1;
TextureID Textures::MINO_J = -1;
TextureID Textures::MINO_L = -1;
TextureID Textures::MINO_O = -1;
TextureID Textures::MINO_S = -1;
TextureID Textures::MINO_T = -1;
TextureID Textures::MINO_Z = -1;
TextureID Textures::MINO_GHOST_I = -1;
TextureID Textures::MINO_GHOST_J = -1;
TextureID Textures::MINO_GHOST_L = -1;
TextureID Textures::MINO_GHOST_O = -1;
TextureID Textures::MINO_GHOST_S = -1;
TextureID Textures::MINO_GHOST_T = -1;
TextureID Textures::MINO_GHOST_Z = -1;
TextureID Textures::MATRIXBG = -1;

void Textures::loadAll(GraphicsContext& gcx)
{
    MINO_I = gcx.loadTexture("data/mino.png", MinoFactory::color(Piece::Type::I));
    MINO_J = gcx.loadTexture("data/mino.png", MinoFactory::color(Piece::Type::J));
    MINO_L = gcx.loadTexture("data/mino.png", MinoFactory::color(Piece::Type::L));
    MINO_O = gcx.loadTexture("data/mino.png", MinoFactory::color(Piece::Type::O));
    MINO_S = gcx.loadTexture("data/mino.png", MinoFactory::color(Piece::Type::S));
    MINO_T = gcx.loadTexture("data/mino.png", MinoFactory::color(Piece::Type::T));
    MINO_Z = gcx.loadTexture("data/mino.png", MinoFactory::color(Piece::Type::Z));
    MINO_GHOST_I = gcx.loadTexture("data/ghost.png", MinoFactory::color(Piece::Type::I));
    MINO_GHOST_J = gcx.loadTexture("data/ghost.png", MinoFactory::color(Piece::Type::J));
    MINO_GHOST_L = gcx.loadTexture("data/ghost.png", MinoFactory::color(Piece::Type::L));
    MINO_GHOST_O = gcx.loadTexture("data/ghost.png", MinoFactory::color(Piece::Type::O));
    MINO_GHOST_S = gcx.loadTexture("data/ghost.png", MinoFactory::color(Piece::Type::S));
    MINO_GHOST_T = gcx.loadTexture("data/ghost.png", MinoFactory::color(Piece::Type::T));
    MINO_GHOST_Z = gcx.loadTexture("data/ghost.png", MinoFactory::color(Piece::Type::Z));
    MATRIXBG = gcx.loadTexture("data/matrix_pattern.png");
}

void Textures::unloadAll(GraphicsContext& gcx)
{
    // unload
    gcx.unloadTexture(MINO_I);
    gcx.unloadTexture(MINO_J);
    gcx.unloadTexture(MINO_L);
    gcx.unloadTexture(MINO_O);
    gcx.unloadTexture(MINO_S);
    gcx.unloadTexture(MINO_T);
    gcx.unloadTexture(MINO_Z);
    gcx.unloadTexture(MINO_GHOST_I);
    gcx.unloadTexture(MINO_GHOST_J);
    gcx.unloadTexture(MINO_GHOST_L);
    gcx.unloadTexture(MINO_GHOST_O);
    gcx.unloadTexture(MINO_GHOST_S);
    gcx.unloadTexture(MINO_GHOST_T);
    gcx.unloadTexture(MINO_GHOST_Z);
    gcx.unloadTexture(MATRIXBG);

    // invalidate
    MINO_I = -1;
    MINO_J = -1;
    MINO_L = -1;
    MINO_O = -1;
    MINO_S = -1;
    MINO_T = -1;
    MINO_Z = -1;
    MINO_GHOST_I = -1;
    MINO_GHOST_J = -1;
    MINO_GHOST_L = -1;
    MINO_GHOST_O = -1;
    MINO_GHOST_S = -1;
    MINO_GHOST_T = -1;
    MINO_GHOST_Z = -1;
    MATRIXBG = -1;
}

} // namespace GameplayResources
