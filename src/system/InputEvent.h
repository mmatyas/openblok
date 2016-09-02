#pragma once

#include <stdint.h>

enum class InputType: uint8_t {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    A,
    B,
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
