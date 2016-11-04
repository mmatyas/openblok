#pragma once

#include <stddef.h>
#include <stdint.h>


enum class InputType: uint8_t {
    GAME_PAUSE,
    GAME_HOLD,
    GAME_HARDDROP,
    GAME_SOFTDROP,
    GAME_MOVE_LEFT,
    GAME_MOVE_RIGHT,
    GAME_ROTATE_LEFT,
    GAME_ROTATE_RIGHT,
    MENU_UP,
    MENU_DOWN,
    MENU_LEFT,
    MENU_RIGHT,
    MENU_OK,
    MENU_CANCEL,
};

struct InputTypeHash {
    template <typename T>
    constexpr size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

class InputEvent {
public:
    explicit InputEvent(InputType type, bool pressed);
    InputType type() const { return m_type; }
    bool down() const { return m_down; }

private:
    InputType m_type;
    bool m_down;
};

enum class WindowEvent : uint8_t {
    RESIZED,
    FOCUS_LOST,
    FOCUS_GAINED,
};

enum class EventType : uint8_t {
    WINDOW,
    INPUT,
};

struct Event {
    EventType type;
    union {
        InputEvent input;
        WindowEvent window;
    };

    Event(InputEvent&&);
    Event(WindowEvent&&);
};
