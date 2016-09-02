#include "InputEvent.h"

InputEvent::InputEvent(InputType type, bool pressed)
    : m_type(type)
    , m_down(pressed)
{}
