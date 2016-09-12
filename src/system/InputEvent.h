#pragma once

#include <stdint.h>

enum class InputType: uint8_t {
    PAUSE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    A,
    B,
    C,
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
