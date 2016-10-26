#include "TSpin.h"

#include "game/components/Piece.h"
#include "game/components/Well.h"

#include <algorithm>
#include <array>


namespace WellComponents {

TSpin::TSpin(bool enabled, bool allow_wall, bool allow_kick)
    : enabled(enabled)
    , allow_wall(allow_wall)
    , allow_kick(allow_kick)
    , allowed(false)
    , last_rotation_point(0)
{
}

void TSpin::clear()
{
    allowed = false;
    last_rotation_point = 0; // this is the default rotation point
}

void TSpin::onWallKick()
{
    last_rotation_point++;
}

void TSpin::onSuccesfulRotation()
{
    assert(allowed == false);
    // rotation by kick may not be a valid tspin
    // if we didn't use wall kick, the rotation point is 0 (default)
    if (last_rotation_point == 0 || allow_kick)
        allowed = true;
}

TSpinDetectionResult TSpin::check(Well& well)
{
    if (!enabled)
        return TSpinDetectionResult::NONE;

    if (well.active_piece->type() != PieceType::T || !allowed)
        return TSpinDetectionResult::NONE;

    // ack
    allowed = false;


    // First, collect the coords we'll have to check,
    // then rotate the coords to the same orientation as the T piece

    // A?B
    // ?T?
    // D?C
    std::array<std::pair<int8_t, uint8_t>, 4> diagonals = {
        std::make_pair(well.active_piece_x, well.active_piece_y),
        std::make_pair(well.active_piece_x + 2, well.active_piece_y),
        std::make_pair(well.active_piece_x + 2, well.active_piece_y + 2),
        std::make_pair(well.active_piece_x, well.active_piece_y + 2),
    };

    auto pattern_orientation = PieceDirection::NORTH;
    while (pattern_orientation != well.active_piece->orientation()) {
        pattern_orientation = nextCW(pattern_orientation);
        std::rotate(diagonals.begin(), diagonals.begin() + 1, diagonals.end());
    }


    // Check if we're in a T-slot:
    // at least 3 of the diagonally adjacent blocks must be occupied

    std::array<bool, 4> diagonals_occupied;
    diagonals_occupied.fill(false);
    for (unsigned i = 0; i < 4; i++) {
        const auto& coord = diagonals.at(i);

        // the walls may not count
        if (coord.first < 0 || coord.first >= static_cast<int>(well.matrix.at(0).size())) {
            if (allow_wall)
                diagonals_occupied[i] = true;
            continue;
        }
        // the bottom layer always counts
        if (coord.second >= well.matrix.size()) {
            diagonals_occupied[i] = true;
            continue;
        }

        if (well.matrix.at(coord.second).at(coord.first).operator bool())
            diagonals_occupied[i] = true;
    }
    if (std::count(diagonals_occupied.begin(), diagonals_occupied.end(), true) < 3)
        return TSpinDetectionResult::NONE;


    // Decide if it's a mini or a proper T-Spin:
    // Proper: front touch OR rotation by p5
    // Mini: back touch (=> only one front)

    bool is_proper_tspin = false;
    const bool front_touch = diagonals_occupied[0] && diagonals_occupied[1];
    if (last_rotation_point == 3 || front_touch)
        is_proper_tspin = true;

    if (is_proper_tspin)
        return TSpinDetectionResult::TSPIN;
    else
        return TSpinDetectionResult::MINI_TSPIN;
}

} // namespace WellComponents
