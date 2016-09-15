#include "PieceFactory.h"

#include <array>
#include <bitset>
#include <assert.h>


std::unique_ptr<Piece> PieceFactory::make_uptr(Piece::Type type)
{
    // SRS rotation - TODO: plugin                                      // AAAABBBBCCCCDDDD
    static const std::array<std::bitset<16>, 4> grid_I = {std::bitset<16>("0000111100000000"),
                                                          std::bitset<16>("0010001000100010"),
                                                          std::bitset<16>("0000000011110000"),
                                                          std::bitset<16>("0100010001000100")};
    static const std::array<std::bitset<16>, 4> grid_J = {std::bitset<16>("1000111000000000"),
                                                          std::bitset<16>("0110010001000000"),
                                                          std::bitset<16>("0000111000100000"),
                                                          std::bitset<16>("0100010011000000")};
    static const std::array<std::bitset<16>, 4> grid_L = {std::bitset<16>("0010111000000000"),
                                                          std::bitset<16>("0100010001100000"),
                                                          std::bitset<16>("0000111010000000"),
                                                          std::bitset<16>("1100010001000000")};
    static const std::array<std::bitset<16>, 4> grid_O = {std::bitset<16>("0110011000000000"),
                                                          std::bitset<16>("0110011000000000"),
                                                          std::bitset<16>("0110011000000000"),
                                                          std::bitset<16>("0110011000000000")};
    static const std::array<std::bitset<16>, 4> grid_S = {std::bitset<16>("0110110000000000"),
                                                          std::bitset<16>("0100011000100000"),
                                                          std::bitset<16>("0000011011000000"),
                                                          std::bitset<16>("1000110001000000")};
    static const std::array<std::bitset<16>, 4> grid_T = {std::bitset<16>("0100111000000000"),
                                                          std::bitset<16>("0100011001000000"),
                                                          std::bitset<16>("0000111001000000"),
                                                          std::bitset<16>("0100110001000000")};
    static const std::array<std::bitset<16>, 4> grid_Z = {std::bitset<16>("1100011000000000"),
                                                          std::bitset<16>("0010011001000000"),
                                                          std::bitset<16>("0000110001100000"),
                                                          std::bitset<16>("0100110010000000")};

    switch(type) {
    case Piece::Type::I : return std::make_unique<Piece>(type, grid_I);
    case Piece::Type::J : return std::make_unique<Piece>(type, grid_J);
    case Piece::Type::L : return std::make_unique<Piece>(type, grid_L);
    case Piece::Type::O : return std::make_unique<Piece>(type, grid_O);
    case Piece::Type::S : return std::make_unique<Piece>(type, grid_S);
    case Piece::Type::T : return std::make_unique<Piece>(type, grid_T);
    case Piece::Type::Z : return std::make_unique<Piece>(type, grid_Z);
    default:
        assert(false);
    }
}
