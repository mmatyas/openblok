#include "Well.h"

#include "MinoFactory.h"
#include "PieceFactory.h"
#include "game/GameState.h"
#include "game/Resources.h"
#include "game/WellEvent.h"
#include "system/EventCollector.h"
#include "system/GraphicsContext.h"
#include "system/InputEvent.h"

#include <assert.h>


Well::Well()
    : gameover(false)
    , active_piece_x(0)
    , active_piece_y(0)
    , ghost_piece_y(0)
    , gravity_delay(GameState::frame_duration * 64)
    , gravity_timer(Duration::zero())
    , horizontal_delay_normal(GameState::frame_duration * 14)
    , horizontal_delay_turbo(GameState::frame_duration * 4)
    , horizontal_delay_current(horizontal_delay_normal)
    , horizontal_timer(Duration::zero())
    , das_timer(horizontal_delay_normal)
    , softdrop_delay(horizontal_delay_turbo)
    , softdrop_timer(Duration::zero())
    , rotation_delay(horizontal_delay_normal)
    , rotation_timer(Duration::zero())
    , lineclear_alpha(GameState::frame_duration * 40, [](double t){
            return static_cast<uint8_t>((1.0 - t) * 0xFF);
        },
        [this](){
            this->removeEmptyRows();
            this->notify(WellEvent::NEXT_REQUESTED);
        })
{
    keystates[InputType::LEFT] = false;
    keystates[InputType::RIGHT] = false;
    keystates[InputType::UP] = false;
    keystates[InputType::DOWN] = false;

    keystates[InputType::A] = false;
    keystates[InputType::B] = false;

    previous_keystates = keystates;

    lineclear_alpha.stop();
}

void Well::updateKeystate(const std::vector<InputEvent>& events)
{
    previous_keystates = keystates;

    for (const auto& event : events)
        keystates[event.type()] = event.down();
}

void Well::handleKeys(const std::vector<InputEvent>& events)
{
    // keep it true only if down key is still down
    skip_gravity = (keystates.at(InputType::DOWN) && previous_keystates.at(InputType::DOWN));

    // for some events onpress/onrelease handling is better suited
    for (const auto& event : events) {
        // press
        if (event.down()) {
            switch (event.type()) {
            case InputType::GAME_HARDDROP:
                hardDrop();
                skip_gravity = true;
                break;

            case InputType::GAME_HOLD:
                notify(WellEvent::HOLD_REQUESTED);
                skip_gravity = true;
                break;

            default:
                break;
            }
        }
        // release
        else {
            switch (event.type()) {
            case InputType::LEFT:
            case InputType::RIGHT:
                resetDAS();
                break;
            default:
                break;
            }
        }
    }


    horizontal_timer -= GameState::frame_duration;
    if (horizontal_timer <= Duration::zero()) {
        if (keystates.at(InputType::LEFT) != keystates.at(InputType::RIGHT)) {
            if (keystates.at(InputType::LEFT))
                moveLeftNow();
            else
                moveRightNow();


            // update DAS
            das_timer -= horizontal_delay_normal;
            if (das_timer < Duration::zero())
                horizontal_delay_current = horizontal_delay_turbo;

            horizontal_timer = horizontal_delay_current;
        }
    }

    rotation_timer -= GameState::frame_duration;
    if (keystates.at(InputType::A) != keystates.at(InputType::B) && rotation_timer <= Duration::zero()) {
        if (keystates.at(InputType::A))
            rotateCCWNow();
        else
            rotateCWNow();

        rotation_timer = rotation_delay;
    }

    softdrop_timer -= GameState::frame_duration;
    if (keystates.at(InputType::DOWN) && softdrop_timer <= Duration::zero()) {
        moveDownNow();
        skip_gravity = true;
        softdrop_timer = softdrop_delay;
    }
}

void Well::resetDAS()
{
    das_timer = horizontal_delay_normal;
    horizontal_delay_current = horizontal_delay_normal;
}

void Well::resetInput()
{
    resetDAS();
    for (auto& key : keystates)
        key.second = false;
}

void Well::updateGravity()
{
    gravity_timer += GameState::frame_duration;
    if (gravity_timer >= gravity_delay) {
        gravity_timer -= gravity_delay;

        // do not apply downward movement twice
        if (!skip_gravity)
            applyGravity();
    }
}

void Well::update(const std::vector<InputEvent>& events, AppContext&)
{
    if (gameover)
        return;

    if (pending_cleared_rows.size()) {
        assert(lineclear_alpha.running());
        lineclear_alpha.update(GameState::frame_duration);
        return;
    }

    // TODO: implement entry delay
    if (!active_piece)
        notify(WellEvent::NEXT_REQUESTED);

    updateKeystate(events);
    handleKeys(events);

    updateGravity();
}

void Well::addPiece(Piece::Type type)
{
    // the player can only control one piece at a time
    assert(!active_piece);

    active_piece = PieceFactory::make_uptr(type);
    active_piece_x = 3;
    active_piece_y = 0;
    calculateGhostOffset();

    if (hasCollisionAt(active_piece_x, active_piece_y)) {
        lockAndReleasePiece();
        gameover = true;
    }
}

void Well::deletePiece()
{
    active_piece = nullptr;
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
            if (matrix.at(row).at(col))
                matrix.at(row).at(col)->draw(gcx,
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

    // Draw line clear animation
    if (pending_cleared_rows.size()) {
        for (auto row : pending_cleared_rows) {
            gcx.drawFilledRect({
                    static_cast<int>(x),
                    static_cast<int>(y + row * Mino::texture_size_px),
                    static_cast<int>(Mino::texture_size_px * matrix.at(0).size()),
                    Mino::texture_size_px
                }, {0xFF, 0xFF, 0xFF, lineclear_alpha.value()});
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
    if (pos_before == active_piece_y)
        lockAndReleasePiece();
}

void Well::hardDrop()
{
    assert(active_piece);

    active_piece_y = ghost_piece_y;
    moveDownNow();
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

void Well::lockAndReleasePiece() {
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
    notify(WellEvent::PIECE_LOCKED);
    checkLineclear();
}

void Well::checkLineclear()
{
    assert(!active_piece);

    for (unsigned row = 0; row < matrix.size(); row++) {
        bool row_filled = true;
        for (auto& cell : matrix[row]) {
            if (!cell) {
                row_filled = false;
                break;
            }
        }

        if (row_filled)
            pending_cleared_rows.insert(row);
    }

    assert(pending_cleared_rows.size() <= 4); // you can clear only 4 rows at once
    if (pending_cleared_rows.size()) {
        for (auto row : pending_cleared_rows) {
            for (auto& cell : matrix[row])
                cell = nullptr;
        }

        lineclear_alpha.reset();
        resetInput();
    }
}

void Well::removeEmptyRows()
{
    // this function should be called if there are empty rows
    assert(pending_cleared_rows.size());
    assert(pending_cleared_rows.size() <= 4);

    switch (pending_cleared_rows.size()) {
        case 1: notify(WellEvent::CLEARED_ONE_LINE); break;
        case 2: notify(WellEvent::CLEARED_TWO_LINES); break;
        case 3: notify(WellEvent::CLEARED_THREE_LINES); break;
        case 4: notify(WellEvent::CLEARED_FOUR_LINES); break;
        default: assert(false);
    }

    for (int row = matrix.size(); row >= 0; row--) {
        if (!pending_cleared_rows.count(row))
            continue;

        int next_filled_row = row;
        while (pending_cleared_rows.count(next_filled_row))
            next_filled_row--;

        if (next_filled_row < 0)
            break;

        matrix[row].swap(matrix[next_filled_row]);
        pending_cleared_rows.insert(next_filled_row);
    }

    pending_cleared_rows.clear();
}

void Well::notify(WellEvent event)
{
    for (const auto& obs : observers[static_cast<uint8_t>(event)])
        obs();
}
