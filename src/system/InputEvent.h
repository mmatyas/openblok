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
