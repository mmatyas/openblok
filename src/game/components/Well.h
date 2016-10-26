#pragma once

#include "WellConfig.h"
#include "game/Matrix.h"
#include "game/Transition.h"
#include "game/WellEvent.h"
#include "well/Ascii.h"
#include "well/AutoRepeat.h"
#include "well/Gravity.h"
#include "well/Render.h"
#include "well/TSpin.h"
#include "system/InputEvent.h"

#include <list>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdint.h>


class AppContext;
class GraphicsContext;
class Mino;
class Piece;
class WellAnimation;
enum class PieceType : uint8_t;


constexpr Duration GRAVITY_20G = Timing::frame_duration_60Hz / 20;


class Well {
public:
    /// Create a new well
    Well();
    Well(WellConfig&&);
    ~Well();

    /// Update the well
    void update(const std::vector<InputEvent>&, AppContext&);

    /// Add a new, player-controllable piece to the well.
    void addPiece(PieceType);
    /// Delete the currently controlled Piece. A new one will be requested eventually.
    void deletePiece();
    /// Returns the current, active piece, controllable by the player.
    /// Can return nullptr, eg. during animations.
    /// This function is only for reading the piece information.
    /// For actual input handling, call GameBoard's functions.
    const std::unique_ptr<Piece>& activePiece() const { return active_piece; }

    /// Set the gravity update rate, in frames
    void setGravity(Duration);
    /// Set the rotation function
    void setRotationFn(std::unique_ptr<RotationFn>&&);

    /// Draw the Minos in the Well
    void drawContent(GraphicsContext&, int x, int y) const;
    /// Draw the background of the Well
    void drawBackground(GraphicsContext&, int x, int y) const;

    /// Register an external event observer.
    template <typename WellObserver>
    void registerObserver(WellEvent::Type evtype, WellObserver&& obs) {
        observers[static_cast<uint8_t>(evtype)].push_back(std::forward<WellObserver>(obs));
    }

    struct {
        WellComponents::AutoRepeat das;
        WellComponents::Gravity gravity;
        WellComponents::Render renderer;
        WellComponents::TSpin tspin;
#ifndef NDEBUG
        WellComponents::Ascii ascii;
#endif
    } components;

#ifndef NDEBUG
    std::string asAscii() const;
    void fromAscii(const std::string&);
#endif

private:
    // true when gameover detected
    bool gameover;

    // the grid matrix
    // TODO: set dimensions from config
    Matrix<std::shared_ptr<Mino>, 22, 10> matrix;

    // the active piece
    int8_t active_piece_x;
    uint8_t active_piece_y;
    uint8_t ghost_piece_y;
    std::unique_ptr<Piece> active_piece;

    // input handling
    std::unordered_map<InputType, bool, InputTypeHash> keystates;
    decltype(keystates) previous_keystates;
    void updateKeystate(const std::vector<InputEvent>&);
    void handleKeys(const std::vector<InputEvent>&);

    // softdrop timers
    Duration softdrop_delay;
    Duration softdrop_timer;

    // active piece movement
    void moveLeftNow();
    void moveRightNow();
    void moveDownNow();
    void hardDrop();
    void rotateCWNow(); // clockwise
    void rotateCCWNow(); // counter-clockwise
    bool placeByWallKick(bool cw_rotation);
    std::unique_ptr<RotationFn> rotation_fn;

    // lock delay
    bool harddrop_locks_instantly;
    bool lock_infinity;
    Transition<void> lock_countdown;
    void updateLockDelay();

    // active piece collision and ghost
    bool isOnGround() const;
    void calculateGhostOffset();
    bool hasCollisionAt(int offset_x, unsigned offset_y) const;
    void lockAndReleasePiece();
    void lockThenRequestNext();

    // line clears
    void checkLineclear();
    void removeEmptyRows();
    std::set<uint8_t> pending_cleared_rows;
    LineClearType last_lineclear_type;

    // listeners
    std::unordered_map<uint8_t, std::vector<std::function<void(const WellEvent&)>>> observers;
    void notify(const WellEvent&);

    // animations
    void updateAnimations();
    std::list<std::unique_ptr<WellAnimation>> animations;
    std::list<std::unique_ptr<WellAnimation>> blocking_anims;

    // TODO: These are the classes that are still too much coupled to the Well
    friend class WellComponents::Ascii;
    friend class WellComponents::Gravity;
    friend class WellComponents::Render;
    friend class WellComponents::TSpin;
};
