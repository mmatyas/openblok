#include "Well.h"

#include "GameState.h"
#include "MinoFactory.h"
#include "PieceFactory.h"
#include "Resources.h"
#include "system/EventCollector.h"
#include "system/GraphicsContext.h"
#include "system/InputEvent.h"
#include "system/Log.h"

#include <chrono>
#include <assert.h>


Well::Well()
    : active_piece_x(0)
    , active_piece_y(0)
    , ghost_piece_y(0)
    , gravity_update_rate(std::chrono::seconds(1))
    , gravity_timer(Duration::zero())
    , autorepeat_delay(std::chrono::milliseconds(300))
    , keypress_normal_update_rate(std::chrono::milliseconds(150))
    , keypress_turbo_update_rate(std::chrono::milliseconds(40))
    , autorepeat_timer(Duration::zero())
    , keypress_rate_now(keypress_normal_update_rate)
    , keypress_countdown(Duration::zero())
{
    keystates[InputType::LEFT] = false;
    keystates[InputType::RIGHT] = false;
    keystates[InputType::UP] = false;
    keystates[InputType::DOWN] = false;

    keystates[InputType::A] = false;
    keystates[InputType::B] = false;
    keystates[InputType::C] = false;

    previous_keystates = keystates;
}

void Well::update(const std::vector<InputEvent>& events, AppContext&)
{
    previous_keystates = keystates;
    for (const auto& event : events) {
        if (keystates.count(event.type()))
            keystates[event.type()] = event.down();
    }

    // true only if down key is still down
    bool movedDown = (keystates.at(InputType::DOWN) && previous_keystates.at(InputType::DOWN));

    // if one of the previously hold buttons was released,
    // reset the autorepeat timer
    for (const auto& item : keystates) {
        if (previous_keystates.at(item.first) && !item.second) {
            reset_autorepeat();
            break;
        }
    }

    keypress_countdown -= GameState::frame_duration;
    if (keypress_countdown <= Duration::zero()) {
        bool keypress_happened = false;
        bool update_autorepeat_timer = false;

        if (keystates.at(InputType::LEFT) != keystates.at(InputType::RIGHT)) {
            if (keystates.at(InputType::LEFT))
                moveLeftNow();
            else
                moveRightNow();

            keypress_happened = true;
            update_autorepeat_timer = true;
        }

        if (keystates.at(InputType::DOWN)) {
            moveDownNow();
            movedDown = true;
            keypress_happened = true;
            update_autorepeat_timer = true;
        }

        if (keystates.at(InputType::UP)) {
            hardDrop();
            movedDown = true;
            keypress_happened = true;
        }

        if (keystates.at(InputType::A) != keystates.at(InputType::B)) {
            if (keystates.at(InputType::A))
                rotateCCWNow();
            else
                rotateCWNow();

            keypress_happened = true;
            reset_autorepeat();
        }

        if (keypress_happened) {
            keypress_countdown = keypress_rate_now;

            // activate turbo mode after some time
            if (update_autorepeat_timer) {
                autorepeat_timer += keypress_rate_now + GameState::frame_duration;
                if (autorepeat_timer > autorepeat_delay)
                    keypress_rate_now = keypress_turbo_update_rate;
            }
            // or reset
            else {
                reset_autorepeat();
            }
        }
    }

    gravity_timer += GameState::frame_duration;
    if (gravity_timer >= gravity_update_rate) {
        gravity_timer -= gravity_update_rate;

        // do not apply downward movement twice
        if (!movedDown)
            applyGravity();
    }
}

void Well::hardDrop()
{
    assert(active_piece);

    active_piece_y = ghost_piece_y;
    moveDownNow();
}

void Well::reset_autorepeat()
{
    autorepeat_timer = Duration::zero();
    keypress_rate_now = keypress_normal_update_rate;
}

bool Well::requiresNewPiece() const
{
    // TODO: update this function when there will be animations
    return !active_piece;
}

void Well::addPiece(Piece::Type type)
{
    // the player can only control one piece at a time
    assert(requiresNewPiece());
    assert(!active_piece);

    active_piece = PieceFactory::make_uptr(type);
    active_piece_x = 3;
    active_piece_y = 0;
    calculateGhostOffset();
}

void Well::fromAscii(const std::string& text)
{
    assert(text.length() == matrix.size() * (matrix[0].size() + 1));

    unsigned str_i = 0;
    for (unsigned row = 0; row < matrix.size(); row++) {
        for (unsigned cell = 0; cell < matrix[0].size(); cell++) {
            if (text.at(str_i) == '.')
                matrix[row][cell].release();
            else
                matrix[row][cell] = MinoFactory::make_uptr(Piece::typeFromAscii(text.at(str_i)));

            str_i++;
        }
        // newline skip
        str_i++;
    }
}

std::string Well::asAscii()
{
    // the piece must be inside the grid, at least partially
    assert(0 <= active_piece_x + 3);
    assert(active_piece_x < static_cast<int>(matrix[0].size()));
    assert(active_piece_y < matrix.size());

    std::string board_layer;
    std::string piece_layer;

    // print board
    for (size_t row = 0; row < matrix.size(); row++) {
        for (size_t cell = 0; cell < matrix[0].size(); cell++) {
            if (matrix[row][cell])
                board_layer += matrix[row][cell]->asAscii();
            else
                board_layer += '.';
        }
        board_layer += '\n';
    }

    // print piece layer
    for (unsigned row = 0; row < matrix.size(); row++) {
        for (unsigned cell = 0; cell < matrix[0].size(); cell++) {
            char appended_char = '.';

            if (active_piece) {
                // if there may be some piece minos (real or ghost) in this column
                if (active_piece_x <= static_cast<int>(cell)
                    && static_cast<int>(cell) <= active_piece_x + 3) {
                    // check ghost first - it should be under the real piece
                    if (ghost_piece_y <= row && row <= ghost_piece_y + 3u) {
                        const auto& mino = active_piece->currentGrid().at(row - ghost_piece_y)
                                                                      .at(cell - active_piece_x);
                        if (mino)
                            appended_char = 'g';
                    }
                    // check piece - overwrite the ascii char even if it has a value
                    if (active_piece_y <= row && row <= active_piece_y + 3u) {
                        const auto& mino = active_piece->currentGrid().at(row - active_piece_y)
                                                                      .at(cell - active_piece_x);
                        if (mino)
                            appended_char = std::tolower(mino->asAscii());
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

void Well::draw(GraphicsContext& gcx, unsigned int x, unsigned int y)
{
    // Draw background
    for (size_t row = 0; row < 22; row++) {
        for (size_t col = 0; col < 10; col++) {
            gcx.drawTexture(TexID::MATRIXBG, {
                static_cast<int>(x + col * Mino::texture_size_px),
                static_cast<int>(y + row * Mino::texture_size_px),
                Mino::texture_size_px,
                Mino::texture_size_px
            });
        }
    }

    // Draw board Minos
    for (size_t row = 0; row < 22; row++) {
        for (size_t col = 0; col < 10; col++) {
            if (matrix[row][col])
                matrix[row][col]->draw(gcx,
                                       x + col * Mino::texture_size_px,
                                       y + row * Mino::texture_size_px);
        }
    }

    // Draw current piece
    if (active_piece) {
        active_piece->draw(gcx,
                           x + active_piece_x * Mino::texture_size_px,
                           y + active_piece_y * Mino::texture_size_px);

        // draw ghost
        for (unsigned row = 0; row < 4; row++) {
            for (unsigned col = 0; col < 4; col++) {
                if (active_piece->currentGrid().at(row).at(col)) {
                    gcx.drawTexture(TexID::MINO_GHOST, {
                        static_cast<int>(x + (active_piece_x + col) * Mino::texture_size_px),
                        static_cast<int>(y + (ghost_piece_y + row) * Mino::texture_size_px),
                        Mino::texture_size_px,
                        Mino::texture_size_px
                    });
                }
            }
        }
    }
}

bool Well::hasCollisionAt(int offset_x, unsigned offset_y)
{
    // At least one line of the piece grid must be on the board.
    // Horizontally, a piece can go between -3 and width+3,
    // vertically from 0 to heigh+3 (it cannot be over the board)
    assert(offset_x + 3 >= 0 && offset_x < static_cast<int>(matrix[0].size()));
    assert(offset_y < matrix.size());
    assert(active_piece);

    size_t piece_gridx = 0, piece_gridy = 0;
    for (unsigned row = offset_y; row <= offset_y + 3; row++) {
        for (int cell = offset_x; cell <= offset_x + 3; cell++) {
            bool board_has_mino_here = true;
            if (row < matrix.size() && cell >= 0 && cell < static_cast<int>(matrix[0].size()))
                board_has_mino_here = matrix.at(row).at(cell).operator bool();

            bool piece_has_mino_here = active_piece->currentGrid()
                                       .at(piece_gridy).at(piece_gridx).operator bool();

            if (piece_has_mino_here && board_has_mino_here)
                return true;

            piece_gridx++;
        }
        piece_gridy++;
        piece_gridx = 0;
    }

    return false;
}

void Well::calculateGhostOffset()
{
    assert(active_piece);

    ghost_piece_y = active_piece_y;
    while (ghost_piece_y + 1u < matrix.size() && !hasCollisionAt(active_piece_x, ghost_piece_y + 1))
        ghost_piece_y++;
}

void Well::applyGravity()
{
    moveDownNow();
}

void Well::moveLeftNow()
{
    if (!active_piece || active_piece_x - 1 <= -3)
        return;

    if (!hasCollisionAt(active_piece_x - 1, active_piece_y)) {
        active_piece_x--;
        calculateGhostOffset();
    }
}

void Well::moveRightNow()
{
    if (!active_piece || active_piece_x + 1 >= static_cast<int>(matrix[0].size()))
        return;

    if (!hasCollisionAt(active_piece_x + 1, active_piece_y)) {
        active_piece_x++;
        calculateGhostOffset();
    }
}

void Well::moveDownNow()
{
    if (!active_piece || active_piece_y + 1u >= matrix.size())
        return;

    auto pos_before = active_piece_y;
    if (!hasCollisionAt(active_piece_x, active_piece_y + 1))
        active_piece_y++;

    // if the position of the piece didn't change,
    // lock the piece and move the minos into the well
    if (pos_before == active_piece_y) {
        for (unsigned row = 0; row < 4; row++) {
            for (unsigned cell = 0; cell < 4; cell++) {
                if (active_piece_y + row < matrix.size()
                    && active_piece_x + cell < matrix.at(0).size()
                    && active_piece->currentGrid().at(row).at(cell))
                {
                    matrix[active_piece_y + row][active_piece_x + cell].swap(
                        active_piece->currentGridMut()[row][cell]
                    );
                }
            }
        }

        active_piece.release();
    }
}

void Well::rotateCWNow()
{
    if (!active_piece)
        return;

    bool success = false;
    active_piece->rotateCW();
    if (!hasCollisionAt(active_piece_x, active_piece_y)) {
        success = true;
    }
    else if (!hasCollisionAt(active_piece_x - 1, active_piece_y)) {
        active_piece_x--;
        success = true;
    }
    else if (!hasCollisionAt(active_piece_x + 1, active_piece_y)) {
        active_piece_x++;
        success = true;
    }

    if (success)
        calculateGhostOffset();
    else
        active_piece->rotateCCW();
}

void Well::rotateCCWNow()
{
    if (!active_piece)
        return;

    bool success = false;
    active_piece->rotateCCW();
    if (!hasCollisionAt(active_piece_x, active_piece_y)) {
        success = true;
    }
    else if (!hasCollisionAt(active_piece_x + 1, active_piece_y)) {
        active_piece_x++;
        success = true;
    }
    else if (!hasCollisionAt(active_piece_x - 1, active_piece_y)) {
        active_piece_x--;
        success = true;
    }

    if (success)
        calculateGhostOffset();
    else
        active_piece->rotateCW();
}
