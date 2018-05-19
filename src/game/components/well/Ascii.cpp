#include "Ascii.h"

#include "game/components/MinoStorage.h"
#include "game/components/Piece.h"
#include "game/components/Well.h"

#include <cctype>


namespace WellComponents {

void Ascii::fromAscii(Well& well, const std::string& text)
{
    assert(text.length() == 22 * (well.matrix[0].size() + 1));

    unsigned str_i = 0;
    for (unsigned row = 18; row < well.matrix.size(); row++) {
        for (unsigned cell = 0; cell < well.matrix[0].size(); cell++) {
            if (text.at(str_i) == '.')
                well.matrix[row][cell] = '\0';
            else
                well.matrix[row][cell] = text.at(str_i);

            str_i++;
        }
        // newline skip
        str_i++;
    }
}

std::string Ascii::asAscii(const Well& well) const
{
    // the piece must be inside the grid, at least partially
    assert(0 <= well.active_piece_x + 3);
    assert(well.active_piece_x < static_cast<int>(well.matrix[0].size()));
    assert(well.active_piece_y < well.matrix.size());

    std::string board_layer;
    std::string piece_layer;

    // print board
    for (size_t row = 18; row < well.matrix.size(); row++) {
        for (size_t cell = 0; cell < well.matrix[0].size(); cell++) {
            if (well.matrix[row][cell])
                board_layer += well.matrix[row][cell];
            else
                board_layer += '.';
        }
        board_layer += '\n';
    }

    // print piece layer
    for (unsigned row = 18; row < well.matrix.size(); row++) {
        for (unsigned cell = 0; cell < well.matrix[0].size(); cell++) {
            char appended_char = '.';

            if (well.active_piece) {
                // if there may be some piece minos (real or ghost) in this column
                if (well.active_piece_x <= static_cast<int>(cell)
                    && static_cast<int>(cell) <= well.active_piece_x + 3) {
                    // check ghost first - it should be under the real piece
                    if (well.ghost_piece_y <= row && row <= well.ghost_piece_y + 3u) {
                        const char mino = well.active_piece->currentGrid().at(row - well.ghost_piece_y)
                                                                           .at(cell - well.active_piece_x);
                        if (mino)
                            appended_char = 'g';
                    }
                    // check piece - overwrite the ascii char even if it has a value
                    if (well.active_piece_y <= row && row <= well.active_piece_y + 3u) {
                        const char has_mino = well.active_piece->currentGrid().at(row - well.active_piece_y)
                                                                              .at(cell - well.active_piece_x);
                        if (has_mino)
                            appended_char = std::tolower(::toAscii(well.active_piece->type()));
                    }
                }
            }

            piece_layer += appended_char;
        }
        piece_layer += '\n';
    }

    assert(board_layer.length() == piece_layer.length());
    std::string output;
    for (size_t i = 0; i < board_layer.length(); i++) {
        if (piece_layer.at(i) != '.') {
            output += piece_layer.at(i);
            continue;
        }

        output += board_layer.at(i);
    }
    return output;
}

} // namespace WellComponents
