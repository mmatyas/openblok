#pragma once

#include "Mino.h"
#include "Piece.h"
#include "game/Matrix.h"
#include "game/Transition.h"
#include "game/WellEvent.h"
#include "system/InputEvent.h"

#include <chrono>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdint.h>


class AppContext;


namespace WellUtil {
class CellLockAnim {
    using Duration = std::chrono::steady_clock::duration;

public:
    CellLockAnim(unsigned row, unsigned col);
    void update(Duration t);
    bool isActive() const { return anim_y_top.running(); }
    void draw(GraphicsContext& gcx, int offset_x, int offset_y) const;

private:
    const unsigned cell_x;
    const unsigned cell_y_top;
    const unsigned cell_y_bottom;

    Transition<int> anim_y_top;
};
} // namespace WellUtil


class Well {
public:
    /// Create a new well
    Well();

    /// Update the well
    void update(const std::vector<InputEvent>&, AppContext&);

    /// Add a new, player-controllable piece to the well.
    void addPiece(Piece::Type);
    /// Delete the currently controlled Piece. A new one will be requested eventually.
    void deletePiece();
    /// Returns the current, active piece, controllable by the player.
    /// Can return nullptr, eg. during animations.
    /// This function is only for reading the piece information.
    /// For actual input handling, call GameBoard's functions.
    const std::unique_ptr<Piece>& activePiece() const { return active_piece; }

    /// Draw the well
    void draw(GraphicsContext&, unsigned x, unsigned y);

    /// Register an external event observer.
    template <typename WellObserver>
    void registerObserver(WellEvent::Type evtype, WellObserver&& obs) {
        observers[static_cast<uint8_t>(evtype)].push_back(std::forward<WellObserver>(obs));
    }

#ifndef NDEBUG
    /// Get the well's string representation.
    /// Can be useful for testing and debugging.
    std::string asAscii();
    /// Set the contents of the well from an Ascii string.
    void fromAscii(const std::string&);
#endif

private:
    using Duration = std::chrono::steady_clock::duration;

    // true when gameover detected
    bool gameover;

    // the grid matrix
    // TODO: set dimensions from config
    Matrix<std::unique_ptr<Mino>, 22, 10> matrix;

    /// Moves the active piece one row down, if it does not collide.
    void applyGravity();

    // the active piece
    int8_t active_piece_x;
    uint8_t active_piece_y;
    uint8_t ghost_piece_y;
    std::unique_ptr<Piece> active_piece;

    // gravity
    bool skip_gravity;
    const Duration gravity_delay;
    Duration gravity_timer;
    void updateGravity();
    bool isOnGround();

    // input handling
    std::unordered_map<InputType, bool, InputTypeHash> keystates;
    decltype(keystates) previous_keystates;
    void updateKeystate(const std::vector<InputEvent>&);
    void handleKeys(const std::vector<InputEvent>&);
    void resetInput(); // reset input (keystate and autorepeat) to default

    // autorepeat horizontal (aka DAS)
    const Duration horizontal_delay_normal; // normal key repeat rate
    const Duration horizontal_delay_turbo; // turbo key repeat rate
    Duration horizontal_delay_current; // current input rate, either normal or turbo
    Duration horizontal_timer; // timer for horizontal autorepeat
    Duration das_timer; // timer for turbo mode activation
    void resetDAS(); // turn off autorepeat mode

    // autorepeat other keys
    const Duration softdrop_delay;
    Duration softdrop_timer;
    const Duration rotation_delay;
    Duration rotation_timer;

    // lock delay
    bool harddrop_locks_instantly;
    bool lock_infinity;
    Transition<void> lock_promise;
    void updateLockDelay();

    // active piece collision and ghost
    void calculateGhostOffset();
    bool hasCollisionAt(int offset_x, unsigned offset_y);
    void lockAndReleasePiece();
    void lockThenRequestNext();

    // active piece movement
    void moveLeftNow();
    void moveRightNow();
    void moveDownNow();
    void hardDrop();
    void rotateCWNow(); // clockwise
    void rotateCCWNow(); // counter-clockwise
    bool placeByWallKick();

    // line clears
    void checkLineclear();
    void removeEmptyRows();
    std::set<uint8_t> pending_cleared_rows;
    Transition<uint8_t> lineclear_alpha;

    // listeners
    std::unordered_map<uint8_t, std::vector<std::function<void(const WellEvent&)>>> observers;
    void notify(const WellEvent&);

    // animations
    std::list<WellUtil::CellLockAnim> onlock_anims;
};
