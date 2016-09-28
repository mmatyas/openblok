#pragma once

#include "Mino.h"
#include "Piece.h"
#include "animations/WellAnimation.h"
#include "game/Matrix.h"
#include "game/Transition.h"
#include "game/WellEvent.h"
#include "system/InputEvent.h"

#include <list>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdint.h>


class AppContext;


constexpr Duration GRAVITY_20G = frame_duration_60Hz / 20;


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

    /// Set the gravity update rate, in frames
    void setGravity(Duration);

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
    // true when gameover detected
    bool gameover;

    // the grid matrix
    // TODO: set dimensions from config
    Matrix<std::unique_ptr<Mino>, 22, 10> matrix;

    // the active piece
    int8_t active_piece_x;
    uint8_t active_piece_y;
    uint8_t ghost_piece_y;
    std::unique_ptr<Piece> active_piece;

    // gravity
    bool skip_gravity;
    Duration gravity_delay;
    Duration gravity_timer;
    void applyGravity(); // moves the active piece one row down, if it does not collide
    void updateGravity(); // updates the gravity timer, and calls applyGravity() if needed

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
    Duration softdrop_delay;
    Duration softdrop_timer;
    const Duration rotation_delay;
    Duration rotation_timer;

    // active piece movement
    void moveLeftNow();
    void moveRightNow();
    void moveDownNow();
    void hardDrop();
    void rotateCWNow(); // clockwise
    void rotateCCWNow(); // counter-clockwise
    bool placeByWallKick();

    // lock delay
    bool harddrop_locks_instantly;
    bool lock_infinity;
    Transition<void> lock_countdown;
    void updateLockDelay();

    // active piece collision and ghost
    bool isOnGround();
    void calculateGhostOffset();
    bool hasCollisionAt(int offset_x, unsigned offset_y);
    void lockAndReleasePiece();
    void lockThenRequestNext();

    // line clears
    void checkLineclear();
    void removeEmptyRows();
    std::set<uint8_t> pending_cleared_rows;

    // listeners
    std::unordered_map<uint8_t, std::vector<std::function<void(const WellEvent&)>>> observers;
    void notify(const WellEvent&);

    // animations
    void updateAnimations();
    std::list<std::unique_ptr<WellAnimation>> animations;
    std::list<std::unique_ptr<WellAnimation>> blocking_anims;
};
