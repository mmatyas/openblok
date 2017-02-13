#include "Well.h"

#include "MinoStorage.h"
#include "Piece.h"
#include "PieceFactory.h"
#include "animations/CellLockAnim.h"
#include "animations/HalfHeightLineClearAnim.h"
#include "animations/LineClearAnim.h"
#include "rotations/RotationFactory.h"
#include "game/Timing.h"
#include "game/WellConfig.h"
#include "game/WellEvent.h"

#include <algorithm>
#include <cstdlib>
#include <assert.h>


Well::Well() : Well(WellConfig()) {}

Well::Well(const WellConfig& config)
    : gameover(false)
    , temporal_disable_timer(Duration::zero())
    , active_piece_x(0)
    , active_piece_y(0)
    , ghost_piece_y(0)
    , softdrop_timer(Duration::zero())
    , last_lineclear_type(LineClearType::NORMAL)
    , das(Timing::frame_duration_60Hz * config.shift_normal,
          Timing::frame_duration_60Hz * config.shift_turbo)
    , lock_delay(*this, Timing::frame_duration_60Hz * config.lock_delay,
                 config.lock_delay_type, config.instant_harddrop)
    , tspin(config.tspin_enabled, config.tspin_allow_wallblock, config.tspin_allow_wallkick)
{
    setGravity(Timing::frame_duration_60Hz * config.starting_gravity);
    rotation_fn = RotationFactory::make(config.rotation_style);
}

Well::~Well() = default;

void Well::updateKeystateOnly(const std::vector<InputEvent>& events)
{
    input.updateKeystate(events);
}

void Well::updateAnimationsOnly()
{
    for (auto& anim : animations)
        anim->update(Timing::frame_duration);

    animations.remove_if([](std::unique_ptr<WellAnimation>& animptr){
        return !animptr->isActive();
    });
}

void Well::updateGameplayOnly(const std::vector<InputEvent>& events)
{
    if (gameover)
        return;

    if (temporal_disable_timer > Duration::zero()) {
        temporal_disable_timer -= Timing::frame_duration;
        return;
    }

    if (pending_cleared_rows.size())
        this->removeEmptyRows();

    if (!active_piece)
        this->notify(WellEvent(WellEvent::Type::NEXT_REQUESTED));

    if (!lock_delay.lockInProgress())
        tspin.clear();

    input.handleKeys(*this, events);
    if (!active_piece)
        return;

    gravity.update(*this);
    lock_delay.update(*this);
}

#ifndef NDEBUG
void Well::update(const std::vector<InputEvent>& events)
{
    updateKeystateOnly(events);
    updateAnimationsOnly();
    updateGameplayOnly(events);
}
#endif

void Well::addPiece(PieceType type)
{
    // the player can only control one piece at a time
    assert(!active_piece);

    active_piece = PieceFactory::make_uptr(type);
    active_piece_x = 3;

    // try to place the piece in row 20, then move up if it fails
    for (active_piece_y = 20; active_piece_y >= 18; active_piece_y--) {
        if (!hasCollisionAt(active_piece_x, active_piece_y)) {
            calculateGhostOffset();
            lock_delay.cancel();
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
    active_piece.reset();
}

void Well::addGarbageLines(unsigned short line_count)
{
    if (!line_count)
        return;

    std::rotate(matrix.begin(), matrix.begin() + line_count, matrix.end());

    size_t gap_location = std::rand() % 10;
    for (size_t row = matrix.size() - line_count; row < matrix.size(); row++) {
        auto& mx_row = matrix.at(row);
        for (size_t col = 0; col < mx_row.size(); col++)
            mx_row.at(col) = MinoStorage::getMino(PieceType::GARBAGE);
        mx_row.at(gap_location).reset();
    }

    if (active_piece)
        calculateGhostOffset();
}

void Well::setGravity(Duration duration)
{
    gravity.setRate(duration);
    softdrop_delay = gravity.currentDelay() / 20;
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

void Well::moveLeftNow()
{
    if (!active_piece || active_piece_x - 1 <= -3)
        return;

    if (!hasCollisionAt(active_piece_x - 1, active_piece_y)) {
        active_piece_x--;
        calculateGhostOffset();
        lock_delay.onHorizontalMove();
        notify(WellEvent(WellEvent::Type::PIECE_MOVED_HORIZONTALLY));
    }
}

void Well::moveRightNow()
{
    if (!active_piece || active_piece_x + 1 >= static_cast<int>(matrix[0].size()))
        return;

    if (!hasCollisionAt(active_piece_x + 1, active_piece_y)) {
        active_piece_x++;
        calculateGhostOffset();
        lock_delay.onHorizontalMove();
        notify(WellEvent(WellEvent::Type::PIECE_MOVED_HORIZONTALLY));
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

    // This function does NOT lock (unless Sonic Drop is active),
    // and the lock delay is activated/updated in the main update().

    if (!isOnGround()) {
        active_piece_y++;
        lock_delay.onDescend(*this);
    }
    else if (lock_delay.sonicLockPossible())
        lockThenRequestNext(); // sonic drop manual lock
}

void Well::hardDrop()
{
    assert(active_piece);

    WellEvent harddrop_event(WellEvent::Type::HARDDROPPED);
    harddrop_event.harddrop.count = ghost_piece_y - active_piece_y;

    active_piece_y = ghost_piece_y;
    moveDownNow();
    if (lock_delay.harddropLocksInstantly())
        lockThenRequestNext();

    notify(harddrop_event);
}

bool Well::placeByWallKick(RotationDirection direction)
{
    assert(active_piece);

    const bool clockwise = (direction == RotationDirection::CLOCKWISE);
    const auto target_rot = active_piece->orientation();
    const auto starting_rot = clockwise ? prevCW(target_rot) : nextCW(target_rot);
    const auto offsets = rotation_fn->possibleOffsets(active_piece->type(), starting_rot, clockwise);

    for (const auto& offset : offsets) {
        tspin.onWallKick();

        if (!hasCollisionAt(active_piece_x + offset.x, active_piece_y + offset.y)) {
            active_piece_x += offset.x;
            active_piece_y += offset.y;
            return true;
        }
    }

    return false;
}

void Well::rotateNow(RotationDirection direction)
{
    if (!active_piece)
        return;

    tspin.clear();

    if (direction == RotationDirection::CLOCKWISE)
        active_piece->rotateCW();
    else
        active_piece->rotateCCW();

    if (hasCollisionAt(active_piece_x, active_piece_y)) {
        if (!placeByWallKick(direction)) {
            if (direction == RotationDirection::CLOCKWISE)
                active_piece->rotateCCW();
            else
                active_piece->rotateCW();
            return;
        }
    }

    calculateGhostOffset();
    tspin.onSuccesfulRotation();
    lock_delay.onSuccesfulRotation();

    notify(WellEvent(WellEvent::Type::PIECE_ROTATED));
}

void Well::lockThenRequestNext()
{
    auto tspin_type = tspin.check(*this);
    lockAndReleasePiece();

    // no line clear happened
    if (pending_cleared_rows.empty()) {
        switch(tspin_type) {
            case TSpinDetectionResult::TSPIN:
                notify(WellEvent(WellEvent::Type::TSPIN_DETECTED));
                break;
            case TSpinDetectionResult::MINI_TSPIN:
                notify(WellEvent(WellEvent::Type::MINI_TSPIN_DETECTED));
                break;
            default:
                break;
        }
        notify(WellEvent(WellEvent::Type::NEXT_REQUESTED));
    }
    // at least one line has been cleared
    else {
        switch(tspin_type) {
            case TSpinDetectionResult::TSPIN:
                assert(pending_cleared_rows.size() < 4);
                last_lineclear_type = LineClearType::TSPIN;
                break;
            case TSpinDetectionResult::MINI_TSPIN:
                assert(pending_cleared_rows.size() < 4);
                last_lineclear_type = LineClearType::MINI_TSPIN;
                break;
            default:
                last_lineclear_type = LineClearType::NORMAL;
                break;
        }

        WellEvent clear_anim_event(WellEvent::Type::LINE_CLEAR_ANIMATION_START);
        clear_anim_event.lineclear.count = pending_cleared_rows.size();
        clear_anim_event.lineclear.type = last_lineclear_type;
        notify(clear_anim_event);
    }
}

/// This function locks the active piece at its current location:
/// moves the minos of the piece into the matrix, fires a PIECE_LOCKED event,
/// and checks if there are clearable lines
void Well::lockAndReleasePiece()
{
    assert(active_piece);
    assert(isOnGround());

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

                if (active_piece_y + row >= 20) {
                    pending_anims.emplace_back(active_piece_y + row - 20,
                                               active_piece_x + cell);
                }
            }
        }
    }

    deletePiece();
    notify(WellEvent(WellEvent::Type::PIECE_LOCKED));

    checkLineclear();
    if (pending_cleared_rows.empty()) {
        // To avoid graphical glitches (animations flying in the air),
        // only add cell lock animation if there was no line clear event
        for (const auto& coord : pending_anims)
            animations.emplace_back(std::make_unique<CellLockAnim>(coord.first, coord.second));
    }
}

/// This function checks if there are fully filled rows, puts them
/// into pending_cleared_rows, and creates the line clear animations
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
                animations.emplace_back(std::make_unique<LineClearAnim>(row));
            else if (row == 1)
                animations.emplace_back(std::make_unique<HalfHeightLineClearAnim>());

            temporal_disable_timer = Timing::frame_duration_60Hz * 40; // TODO: make this configurable
        }
    }
}

/// This function consumes the lines previously stored in pending_cleared_rows,
/// and fires the LINE_CLEAR event
void Well::removeEmptyRows()
{
    // this function should be called if there are empty rows
    assert(pending_cleared_rows.size());
    assert(pending_cleared_rows.size() <= 4);

    WellEvent clear_event(WellEvent::Type::LINE_CLEAR);
    clear_event.lineclear.count = pending_cleared_rows.size();
    clear_event.lineclear.type = last_lineclear_type;
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

std::string Well::asAscii() const
{
    return ascii.asAscii(*this);
}

void Well::fromAscii(const std::string& text)
{
    ascii.fromAscii(*this, text);
}

#endif

void Well::drawContent(GraphicsContext& gcx, int x, int y) const
{
    renderer.drawContent(*this, gcx, x, y);
}
