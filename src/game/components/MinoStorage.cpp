#include "MinoStorage.h"

#include "Mino.h"
#include "game/AppContext.h"
#include "system/GraphicsContext.h"

#include <assert.h>


std::unordered_map<PieceType, std::shared_ptr<Mino>, PieceTypeHash> MinoStorage::minos;
std::unordered_map<PieceType, std::shared_ptr<Mino>, PieceTypeHash> MinoStorage::ghosts;
std::shared_ptr<Mino> MinoStorage::matrixcell;

#ifndef NDEBUG
void MinoStorage::loadDummyMinos()
{
    for (const auto& type : PieceTypeList) {
        minos[type] = std::make_shared<Mino>(nullptr, ::toAscii(type));
        ghosts[type] = std::make_shared<Mino>(nullptr, 'g');
    }
    minos[PieceType::GARBAGE] = std::make_shared<Mino>(nullptr, ::toAscii(PieceType::GARBAGE));
    matrixcell.reset();
}
#endif

void MinoStorage::loadMinos(AppContext& app)
{
    if (app.theme().gameplay.custom_minos)
        loadCustomMinos(app);
    else
        loadTintedMinos(app.gcx(), app.theme().get_texture("mino.png"));
}

void MinoStorage::loadTintedMinos(GraphicsContext& gcx, const std::string& path)
{
    minos[PieceType::GARBAGE] = std::make_shared<Mino>(gcx.loadTexture(path), ::toAscii(PieceType::GARBAGE));
    for (const auto& type : PieceTypeList)
        minos[type] = std::make_shared<Mino>(gcx.loadTexture(path, color(type)), ::toAscii(type));
}

void MinoStorage::loadCustomMinos(AppContext& app)
{
    static const std::unordered_map<PieceType, const std::string, PieceTypeHash> suffixes = {
        { PieceType::I, "i" },
        { PieceType::J, "j" },
        { PieceType::L, "l" },
        { PieceType::O, "o" },
        { PieceType::S, "s" },
        { PieceType::T, "t" },
        { PieceType::Z, "z" },
        { PieceType::GARBAGE, "garbage" },
    };
    for (const auto& pair : suffixes) {
        const PieceType type = pair.first;
        try {
            const std::string path = app.theme().get_texture("mino_" + pair.second + ".png");
            minos[type] = std::make_shared<Mino>(app.gcx().loadTexture(path), ::toAscii(type));
        }
        catch (const std::runtime_error& err) {
            // fallback to regular mino
            minos[type] = std::make_shared<Mino>(app.gcx().loadTexture(
                app.theme().get_texture("mino.png")), ::toAscii(type));
        }
    }
}

void MinoStorage::loadGhosts(AppContext& app)
{
    const auto path = app.theme().get_texture("ghost.png");

    if (app.theme().gameplay.tint_ghost)
        loadTintedGhosts(app.gcx(), path);
    else
        loadSimpleGhosts(app.gcx(), path);
}

void MinoStorage::loadTintedGhosts(GraphicsContext& gcx, const std::string& path)
{
    for (const auto& type : PieceTypeList)
        ghosts[type] = std::make_shared<Mino>(gcx.loadTexture(path, color(type)), 'g');
}

void MinoStorage::loadSimpleGhosts(GraphicsContext& gcx, const std::string& path)
{
    for (const auto& type : PieceTypeList)
        ghosts[type] = std::make_shared<Mino>(gcx.loadTexture(path), 'g');
}

void MinoStorage::loadMatrixCell(GraphicsContext& gcx, const std::string& path)
{
    matrixcell = std::make_shared<Mino>(gcx.loadTexture(path), '.');
}

std::shared_ptr<Mino> MinoStorage::getMino(PieceType type)
{
    assert(minos.count(type));
    return minos.at(type);
}

std::shared_ptr<Mino> MinoStorage::getGhost(PieceType type)
{
    assert(ghosts.count(type));
    return ghosts.at(type);
}

std::shared_ptr<Mino> MinoStorage::getMatrixCell()
{
    assert(matrixcell);
    return matrixcell;
}

RGBColor MinoStorage::color(PieceType type)
{
    switch(type) {
    case PieceType::I : return 0x00FFFF_rgb;
    case PieceType::J : return 0x0040FF_rgb;
    case PieceType::L : return 0xFFA500_rgb;
    case PieceType::O : return 0xFFFF00_rgb;
    case PieceType::S : return 0x80FF00_rgb;
    case PieceType::T : return 0xAA00FF_rgb;
    case PieceType::Z : return 0xFF0000_rgb;
    case PieceType::GARBAGE : return 0xFFFFFF_rgb;
    }
    assert(false);
}
