#pragma once

#include "game/Matrix.h"
#include "game/WellEvent.h"
#include "well/AutoRepeat.h"
#include "well/Input.h"
#include "well/Gravity.h"
#include "well/LockDelay.h"
#include "well/Render.h"
#include "well/TSpin.h"

#include <list>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>
#include <stdint.h>

#ifndef NDEBUG
#include "well/Ascii.h"
#include <string>
#endif


class AppContext;
class GraphicsContext;
class Mino;
class Piece;
class RotationFn;
class WellAnimation;
struct WellConfig;
enum class PieceType : uint8_t;


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
    /// For actual input handling, call Well's update method.
    const std::unique_ptr<Piece>& activePiece() const { return active_piece; }

    /// Set the gravity update rate
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

#ifndef NDEBUG
    std::string asAscii() const;
    void fromAscii(const std::string&);
#endif

private:
    // true when gameover detected
    bool gameover;
    // input is temporally disabled, eg, during blocking animations
    Duration temporal_disable_timer;

    // the grid matrix
    // TODO: set dimensions from config
    Matrix<std::shared_ptr<Mino>, 22, 10> matrix;

    // the active piece
    int8_t active_piece_x;
    uint8_t active_piece_y;
    uint8_t ghost_piece_y;
    std::unique_ptr<Piece> active_piece;

    // softdrop timers
    Duration softdrop_delay;
    Duration softdrop_timer;

    // active piece movement
    enum class RotationDirection : uint8_t { CLOCKWISE, COUNTER_CLOCKWISE };
    void moveLeftNow();
    void moveRightNow();
    void moveDownNow();
    void hardDrop();
    void rotateNow(RotationDirection);
    bool placeByWallKick(RotationDirection);
    std::unique_ptr<RotationFn> rotation_fn;

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

    // components
    WellComponents::AutoRepeat das;
    WellComponents::Gravity gravity;
    WellComponents::Input input;
    WellComponents::LockDelay lock_delay;
    WellComponents::Render renderer;
    WellComponents::TSpin tspin;
#ifndef NDEBUG
    WellComponents::Ascii ascii;
#endif

    // TODO: These are the classes that are still too much coupled to the Well
    friend class WellComponents::Gravity;
    friend class WellComponents::Input;
    friend class WellComponents::LockDelay;
    friend class WellComponents::Render;
    friend class WellComponents::TSpin;
#ifndef NDEBUG
    friend class WellComponents::Ascii;
#endif
};
