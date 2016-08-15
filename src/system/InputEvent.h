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
    explicit InputEvent(InputType type);
    InputType type() { return m_type; }

private:
    InputType m_type;
};
