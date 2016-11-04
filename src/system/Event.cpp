#include "Event.h"

InputEvent::InputEvent(InputType type, bool pressed)
    : m_type(type)
    , m_down(pressed)
{}

Event::Event(InputEvent&& inputev)
    : type(EventType::INPUT)
    , input(inputev)
{}

Event::Event(WindowEvent&& windowev)
    : type(EventType::WINDOW)
    , window(windowev)
{}
