#include "MinoStorage.h"

#include "Mino.h"
#include "system/GraphicsContext.h"

#include <assert.h>


std::unordered_map<PieceType, std::shared_ptr<Mino>, PieceTypeHash> MinoStorage::minos;
std::unordered_map<PieceType, std::shared_ptr<Mino>, PieceTypeHash> MinoStorage::ghosts;
std::shared_ptr<Mino> MinoStorage::matrixcell;


void MinoStorage::loadTintedMinos(GraphicsContext& gcx, const std::string& path)
{
    for (const auto& type : PieceTypeList)
        minos[type] = std::make_shared<Mino>(gcx.loadTexture(path, color(type)), ascii(type));
}

void MinoStorage::loadTintedGhosts(GraphicsContext& gcx, const std::string& path)
{
    for (const auto& type : PieceTypeList)
        ghosts[type] = std::make_shared<Mino>(gcx.loadTexture(path, color(type)), 'g');
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
    case PieceType::J : return 0x0000FF_rgb;
    case PieceType::L : return 0xFFA500_rgb;
    case PieceType::O : return 0xFFFF00_rgb;
    case PieceType::S : return 0x80FF00_rgb;
    case PieceType::T : return 0xAA00FF_rgb;
    case PieceType::Z : return 0xFF0000_rgb;
    }
    assert(false);
}

char MinoStorage::ascii(PieceType type)
{
    switch(type) {
    case PieceType::I : return 'I';
    case PieceType::J : return 'J';
    case PieceType::L : return 'L';
    case PieceType::O : return 'O';
    case PieceType::S : return 'S';
    case PieceType::T : return 'T';
    case PieceType::Z : return 'Z';
    }
    assert(false);
}
