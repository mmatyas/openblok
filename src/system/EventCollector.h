#pragma once

#include "InputEvent.h"

#include <memory>
#include <vector>

/// Native event handling interface.
///
/// The implementation should query the native events received by
/// the application, and prepare them to be usable by other parts of the game.
/// It should detect input events (eg. keyboard key presses), and detect
/// when a user wants to quit the program (eg. close button, Alt-F4).
class EventCollector {
public:
    virtual ~EventCollector() {};

    /// In every frame, the EventCollector should receive the available
    /// native events, and based on that, return a list of usable game events.
    /// If the user wants to quit the game by a native event, then after this call
    /// `quit_requested()` should return true.
    virtual std::vector<InputEvent> collect() = 0;

    /// Return `true` if the user wants to quit the program, eg. by closing the game
    /// window or pressing certain key combinations (Alt-F4, Ctrl-Q, ...).
    virtual bool quit_requested() = 0;

private:
    static std::unique_ptr<EventCollector> create();

friend class AppContext;
};
