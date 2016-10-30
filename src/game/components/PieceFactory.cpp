#include "PieceFactory.h"

#include <assert.h>


std::map<PieceType, std::array<std::bitset<16>, 4>> PieceFactory::initial_positions;

void PieceFactory::changeInitialPositions(std::map<PieceType, std::array<std::bitset<16>, 4>>&& mapping)
{
    initial_positions.swap(mapping);
}


std::unique_ptr<Piece> PieceFactory::make_uptr(PieceType type)
{
    assert(initial_positions.count(type));
    return std::make_unique<Piece>(type, initial_positions.at(type));
}
