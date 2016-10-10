#pragma once

#include <array>
#include <stdint.h>


enum class PieceType : uint8_t {
    I, J, L, O, S, T, Z
};

constexpr std::array<PieceType, 7> PieceTypeList = {
    PieceType::I,
    PieceType::J,
    PieceType::L,
    PieceType::O,
    PieceType::S,
    PieceType::T,
    PieceType::Z,
};

struct PieceTypeHash {
    size_t operator()(PieceType t) const {
        return static_cast<size_t>(t);
    }
};
