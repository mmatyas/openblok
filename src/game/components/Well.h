#pragma once

#include "Mino.h"
#include "Piece.h"
#include "game/Matrix.h"
#include "game/Transition.h"
#include "system/InputEvent.h"

#include <chrono>
#include <memory>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include <stdint.h>


class AppContext;
enum class WellEvent: uint8_t;

class GraphicsContext;
namespace SuiteWell {
    class WellFixtureMoveHelper;
    class WellFixtureRotateHelper;
}

class Well {
public:
    /// Create a new well
    Well();

    /// Update the well
    void update(const std::vector<InputEvent>&, AppContext&);

    /// Moves the active piece one row down, if it does not collide.
    void applyGravity();

    /// Add a new, player-controllable piece to the well.
    void addPiece(Piece::Type);
    /// Delete the currently controlled Piece. A new one will be requested eventually.
    void deletePiece();
    /// Returns the current, active piece, controllable by the player.
    /// Can return nullptr, eg. during animations.
    /// This function is only for reading the piece information.
    /// For actual input handling, call GameBoard's functions.
    const std::unique_ptr<Piece>& activePiece() const { return active_piece; }

    /// Register an external event observer.
    template <typename WellObserver>
    void registerObserver(WellEvent event, WellObserver&& obs) {
        observers[static_cast<uint8_t>(event)].push_back(std::forward<WellObserver>(obs));
    }

    /// Get the well's string representation.
    /// Can be useful for testing and debugging.
    std::string asAscii();
    /// Set the contents of the well from an Ascii string.
    void fromAscii(const std::string&);

    /// Draw the well
    void draw(GraphicsContext&, unsigned x, unsigned y);

private:
    using Duration = std::chrono::steady_clock::duration;

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
    const Duration gravity_delay;
    Duration gravity_timer;
    void updateGravity();

    // input handling
    std::unordered_map<InputType, bool, InputTypeHash> keystates;
    decltype(keystates) previous_keystates;
    void updateKeystate(const std::vector<InputEvent>&);
    void handleKeys(const std::vector<InputEvent>&);

    // autorepeat horizontal (aka DAS)
    const Duration horizontal_delay_normal; // normal key repeat rate
    const Duration horizontal_delay_turbo; // turbo key repeat rate
    Duration horizontal_delay_current; // current input rate, either normal or turbo
    Duration horizontal_timer; // timer for horizontal autorepeat
    const Duration das_delay; // the time it takes to activate turbo mode
    Duration das_timer; // timer for turbo mode activation
    void resetDAS(); // turn off autorepeat mode
    void resetInput(); // reset input (keystate and autorepeat) to default

    // autorepeat other keys
    const Duration softdrop_delay;
    Duration softdrop_timer;
    const Duration rotation_delay;
    Duration rotation_timer;

    // active piece collision and ghost
    void calculateGhostOffset();
    bool hasCollisionAt(int offset_x, unsigned offset_y);
    void lockAndReleasePiece();

    // active piece movement
    void moveLeftNow();
    void moveRightNow();
    void moveDownNow();
    void hardDrop();
    void rotateCWNow(); // clockwise
    void rotateCCWNow(); // counter-clockwise

    // line clears
    void checkLineclear();
    void removeEmptyRows();
    std::set<uint8_t> pending_cleared_rows;
    Transition<uint8_t> lineclear_alpha;

    // listeners
    std::unordered_map<uint8_t, std::vector<std::function<void()>>> observers;
    void notify(WellEvent);

friend class SuiteWell::WellFixtureMoveHelper;
friend class SuiteWell::WellFixtureRotateHelper;
};
