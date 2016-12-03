#include "Event.h"

InputEvent::InputEvent(InputType type, bool pressed, DeviceID source)
    : m_type(type)
    , m_down(pressed)
    , m_src_device_id(source)
{}

Event::Event(InputEvent&& inputev)
    : type(EventType::INPUT)
    , input(inputev)
{}

Event::Event(WindowEvent&& windowev)
    : type(EventType::WINDOW)
    , window(windowev)
{}
