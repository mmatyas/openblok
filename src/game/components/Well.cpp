#include "Well.h"

#include "Mino.h"
#include "MinoStorage.h"
#include "Piece.h"
#include "PieceFactory.h"
#include "animations/CellLockAnim.h"
#include "animations/LineClearAnim.h"
#include "game/Timing.h"
#include "game/WellEvent.h"

#include <assert.h>


Well::Well() : Well(WellConfig()) {}

Well::Well(WellConfig&& config)
    : gameover(false)
    , active_piece_x(0)
    , active_piece_y(0)
    , ghost_piece_y(0)
    , gravity_delay(Timing::frame_duration_60Hz * config.starting_gravity)
    , gravity_timer(Duration::zero())
    , horizontal_delay_normal(Timing::frame_duration_60Hz * config.shift_normal)
    , horizontal_delay_turbo(Timing::frame_duration_60Hz * config.shift_turbo)
    , horizontal_delay_current(horizontal_delay_normal)
    , horizontal_timer(Duration::zero())
    , das_timer(horizontal_delay_normal)
    , softdrop_delay(gravity_delay / 20)
    , softdrop_timer(Duration::zero())
    , rotation_delay(Timing::frame_duration_60Hz * config.rotation_delay)
    , rotation_timer(Duration::zero())
    , rotation_fn(std::move(config.rotation_fn))
    , harddrop_locks_instantly(config.instant_harddrop)
    , lock_infinity(config.infinity_lock)
    , lock_countdown(Timing::frame_duration_60Hz * config.lock_delay, [](double){}, [this](){
            this->lockThenRequestNext();
        })
{
    keystates[InputType::GAME_MOVE_LEFT] = false;
    keystates[InputType::GAME_MOVE_RIGHT] = false;
    keystates[InputType::GAME_SOFTDROP] = false;
    keystates[InputType::GAME_ROTATE_LEFT] = false;
    keystates[InputType::GAME_ROTATE_RIGHT] = false;
    previous_keystates = keystates;
}

Well::~Well() = default;

void Well::update(const std::vector<InputEvent>& events, AppContext&)
{
    updateKeystate(events);
    updateAnimations();

    if (gameover)
        return;

    if (pending_cleared_rows.size()) {
        // when the animation has ended, but the rows weren't removed yet
        if (blocking_anims.empty()) {
            this->removeEmptyRows();
            this->lock_countdown.stop();
            this->notify(WellEvent(WellEvent::Type::NEXT_REQUESTED));
        }
        return;
    }

    handleKeys(events);

    if (!active_piece)
        return;

    updateGravity();
    updateLockDelay();
}

void Well::updateAnimations()
{
    for (auto& anim : animations)
        anim->update(Timing::frame_duration);
    animations.remove_if([](std::unique_ptr<WellAnimation>& animptr){
        return !animptr->isActive();
    });

    for (auto& anim : blocking_anims)
        anim->update(Timing::frame_duration);
    blocking_anims.remove_if([](std::unique_ptr<WellAnimation>& animptr){
        return !animptr->isActive();
    });
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
    skip_gravity = (keystates.at(InputType::GAME_SOFTDROP)
                    && previous_keystates.at(InputType::GAME_SOFTDROP));

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
                notify(WellEvent(WellEvent::Type::HOLD_REQUESTED));
                skip_gravity = true;
                break;

            default:
                break;
            }
        }
        // release
        else {
            switch (event.type()) {
            case InputType::GAME_MOVE_LEFT:
            case InputType::GAME_MOVE_RIGHT:
                resetDAS();
                break;
            default:
                break;
            }
        }
    }


    rotation_timer -= Timing::frame_duration;
    if (keystates.at(InputType::GAME_ROTATE_LEFT) != keystates.at(InputType::GAME_ROTATE_RIGHT)
        && rotation_timer <= Duration::zero()) {
        if (keystates.at(InputType::GAME_ROTATE_LEFT))
            rotateCCWNow();
        else
            rotateCWNow();

        rotation_timer = rotation_delay;
        notify(WellEvent(WellEvent::Type::PIECE_ROTATED));
    }

    horizontal_timer -= Timing::frame_duration;
    if (horizontal_timer <= Duration::zero()) {
        if (keystates.at(InputType::GAME_MOVE_LEFT) != keystates.at(InputType::GAME_MOVE_RIGHT)) {
            if (keystates.at(InputType::GAME_MOVE_LEFT))
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

    softdrop_timer -= Timing::frame_duration;
    if (keystates.at(InputType::GAME_SOFTDROP) && softdrop_timer <= Duration::zero()) {
        moveDownNow();
        skip_gravity = true;
        softdrop_timer = softdrop_delay;
        if (active_piece && !lock_countdown.running())
            notify(WellEvent(WellEvent::Type::SOFTDROPPED));
    }
}

void Well::resetDAS()
{
    das_timer = horizontal_delay_normal;
    horizontal_delay_current = horizontal_delay_normal;
}

void Well::updateGravity()
{
    gravity_timer += Timing::frame_duration;
    while (gravity_timer >= gravity_delay) {
        gravity_timer -= gravity_delay;

        // do not apply downward movement twice
        if (!skip_gravity)
            applyGravity();
    }
}

void Well::updateLockDelay()
{
    if (isOnGround())
        lock_countdown.unpause();
    else
        lock_countdown.stop();
    lock_countdown.update(Timing::frame_duration);
}

void Well::addPiece(PieceType type)
{
    // the player can only control one piece at a time
    assert(!active_piece);

    active_piece = PieceFactory::make_uptr(type);
    active_piece_x = 3;

    // try to place the piece in row 20, then move up if it fails
    for (active_piece_y = 3; active_piece_y > 0; /* note: unsigned */ ) {
        if (!hasCollisionAt(active_piece_x, --active_piece_y)) {
            calculateGhostOffset();
            return;
        }
    }

    // couldn't place the piece, game over
    lockAndReleasePiece();
    gameover = true;
    notify(WellEvent(WellEvent::Type::GAME_OVER));
}

void Well::deletePiece()
{
    active_piece = nullptr;
}

void Well::setGravity(Duration duration)
{
    // do not go below 20G
    gravity_delay = std::max<Duration>(duration, GRAVITY_20G);
    softdrop_delay = gravity_delay / 20;
}

void Well::setRotationFn(std::unique_ptr<RotationFn>&& fn)
{
    rotation_fn.swap(fn);
}

bool Well::hasCollisionAt(int offset_x, unsigned offset_y) const
{
    // At least one line of the piece grid must be on the board.
    // Horizontally, a piece can go between -3 and width+3,
    // vertically from 0 to heigh+3 (it cannot be over the board)
    if (offset_x + 3 < 0 || offset_x >= static_cast<int>(matrix.at(0).size()))
        return true;

    if (offset_y >= matrix.size())
        return true;

    assert(active_piece);

    size_t piece_gridx = 0, piece_gridy = 0;
    for (unsigned row = offset_y; row <= offset_y + 3; row++) {
        for (int cell = offset_x; cell <= offset_x + 3; cell++) {
            bool board_has_mino_here = true;
            if (row < matrix.size() &&
                cell >= 0 &&
                cell < static_cast<int>(matrix.at(0).size()))
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

        if (lock_infinity)
            lock_countdown.stop();
    }
}

void Well::moveRightNow()
{
    if (!active_piece || active_piece_x + 1 >= static_cast<int>(matrix[0].size()))
        return;

    if (!hasCollisionAt(active_piece_x + 1, active_piece_y)) {
        active_piece_x++;
        calculateGhostOffset();

        if (lock_infinity)
            lock_countdown.stop();
    }
}

bool Well::isOnGround() const
{
    assert(active_piece);
    assert(active_piece_y + 1u < matrix.size());

    return hasCollisionAt(active_piece_x, active_piece_y + 1);
}

void Well::moveDownNow()
{
    if (!active_piece || active_piece_y + 1u >= matrix.size())
        return;

    if (!isOnGround())
        active_piece_y++;
    else if (!harddrop_locks_instantly && lock_countdown.running()) {
        // sonic drop on-demand lock
        lockThenRequestNext();
    }
}

void Well::hardDrop()
{
    assert(active_piece);

    WellEvent harddrop_event(WellEvent::Type::HARDDROPPED);
    harddrop_event.count = ghost_piece_y - active_piece_y;

    active_piece_y = ghost_piece_y;
    moveDownNow();
    if (harddrop_locks_instantly)
        lockThenRequestNext();

    notify(harddrop_event);
}

bool Well::placeByWallKick(bool cw_rotation)
{
    assert(active_piece);

    const auto target_rot = active_piece->orientation();
    const auto starting_rot = cw_rotation ? prevCW(target_rot) : nextCW(target_rot);
    const auto offsets = rotation_fn->call(active_piece->type(), starting_rot, cw_rotation);

    for (const auto& offset : offsets) {
        if (!hasCollisionAt(active_piece_x + offset.first, active_piece_y + offset.second)) {
            active_piece_x += offset.first;
            active_piece_y += offset.second;
            return true;
        }
    }

    return false;
}

void Well::rotateCWNow()
{
    if (!active_piece)
        return;

    active_piece->rotateCW();
    if (hasCollisionAt(active_piece_x, active_piece_y)) {
        if (!placeByWallKick(true)) {
            active_piece->rotateCCW();
            return;
        }
    }

    calculateGhostOffset();

    if (lock_infinity)
        lock_countdown.stop();
}

void Well::rotateCCWNow()
{
    if (!active_piece)
        return;

    active_piece->rotateCCW();
    if (hasCollisionAt(active_piece_x, active_piece_y)) {
        if (!placeByWallKick(false)) {
            active_piece->rotateCW();
            return;
        }
    }

    calculateGhostOffset();

    if (lock_infinity)
        lock_countdown.stop();
}

void Well::lockThenRequestNext()
{
    lockAndReleasePiece();
    if (!gameover && blocking_anims.empty())
        notify(WellEvent(WellEvent::Type::NEXT_REQUESTED));
}

void Well::lockAndReleasePiece()
{
    std::list<std::pair<unsigned, unsigned>> pending_anims;

    for (unsigned row = 0; row < 4; row++) {
        for (unsigned cell = 0; cell < 4; cell++) {
            if (active_piece_y + row >= matrix.size() ||
                active_piece_x + cell >= matrix.at(0).size() ||
                active_piece_x + static_cast<int>(cell) < 0)
                continue;

            if (active_piece->currentGrid().at(row).at(cell)) {
                matrix[active_piece_y + row][active_piece_x + cell].swap(
                    active_piece->currentGridMut()[row][cell]
                );

                if (active_piece_y + row >= 2) {
                    pending_anims.emplace_back(active_piece_y + row - 2,
                                               active_piece_x + cell);
                }
            }
        }
    }

    active_piece.reset();
    lock_countdown.stop();
    notify(WellEvent(WellEvent::Type::PIECE_LOCKED));

    checkLineclear();
    if (pending_cleared_rows.empty()) {
        // To avoid graphical glitches (animations flying in the air),
        // only add cell lock animation if there was no line clear event
        for (const auto& coord : pending_anims)
            animations.emplace_back(std::make_unique<CellLockAnim>(coord.first, coord.second));
    }
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

            if (row >= 2)
                blocking_anims.emplace_back(std::make_unique<LineClearAnim>(row));
        }

        WellEvent clear_anim_event(WellEvent::Type::LINE_CLEAR_ANIMATION_START);
        clear_anim_event.count = pending_cleared_rows.size();
        notify(clear_anim_event);
    }
}

void Well::removeEmptyRows()
{
    // this function should be called if there are empty rows
    assert(pending_cleared_rows.size());
    assert(pending_cleared_rows.size() <= 4);

    WellEvent clear_event(WellEvent::Type::LINE_CLEAR);
    clear_event.count = pending_cleared_rows.size();
    notify(clear_event);

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

void Well::notify(const WellEvent& event)
{
    for (const auto& obs : observers[static_cast<uint8_t>(event.type)])
        obs(event);
}

#ifndef NDEBUG

void Well::fromAscii(const std::string& text)
{
    assert(text.length() == matrix.size() * (matrix[0].size() + 1));

    unsigned str_i = 0;
    for (unsigned row = 0; row < matrix.size(); row++) {
        for (unsigned cell = 0; cell < matrix[0].size(); cell++) {
            if (text.at(str_i) == '.')
                matrix[row][cell].reset();
            else
                matrix[row][cell] = MinoStorage::getMino(Piece::typeFromAscii(text.at(str_i)));

            str_i++;
        }
        // newline skip
        str_i++;
    }
}

std::string Well::asAscii() const
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

#endif

void Well::drawBackground(int x, int y) const
{
    const auto& cell = MinoStorage::getMatrixCell();
    for (size_t row = 0; row < 20; row++) {
        for (size_t col = 0; col < 10; col++) {
            cell->draw(x + col * Mino::texture_size_px,
                       y + row * Mino::texture_size_px);
        }
    }
}

void Well::drawContent(GraphicsContext& gcx, int x, int y) const
{
    // Draw board Minos
    for (size_t row = 0; row < 20; row++) {
        for (size_t col = 0; col < 10; col++) {
            const auto& cell = matrix.at(row + 2).at(col);
            if (cell) {
                cell->draw(x + col * Mino::texture_size_px,
                           y + row * Mino::texture_size_px);
            }
        }
    }

    // Draw current piece
    if (active_piece) {
        // draw ghost
        for (unsigned row = 0; row < 4; row++) {
            if (ghost_piece_y + row < 2) // hide rows 21-22
                continue;
            for (unsigned col = 0; col < 4; col++) {
                if (active_piece->currentGrid().at(row).at(col)) {
                    const auto& cell = MinoStorage::getGhost(active_piece->type());
                    cell->draw(x + (active_piece_x + col) * Mino::texture_size_px,
                               y + (ghost_piece_y + row - 2) * Mino::texture_size_px);
                }
            }
        }

        // draw piece
        for (unsigned row = 0; row < 4; row++) {
            if (active_piece_y + row < 2) // hide rows 21-22
                continue;
            for (unsigned col = 0; col < 4; col++) {
                const auto& cell = active_piece->currentGrid().at(row).at(col);
                if (cell) {
                    cell->draw(x + (active_piece_x + col) * Mino::texture_size_px,
                               y + (active_piece_y + row - 2) * Mino::texture_size_px);
                }
            }
        }
    }

    // Draw animations
    for (auto& anim : animations)
        anim->draw(gcx, x, y);
    for (auto& anim : blocking_anims)
        anim->draw(gcx, x, y);
}
