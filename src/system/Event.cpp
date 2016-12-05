#include "Event.h"

InputEvent::InputEvent(InputType type, bool pressed, DeviceID source)
    : m_type(type)
    , m_down(pressed)
    , m_src_device_id(source)
{}

DeviceEvent::DeviceEvent(DeviceEventType type, int device_id)
    : type(type), device_id(device_id)
{}

RawInputEvent::RawInputEvent(int device_id, uint16_t button, bool down)
    : device_id(device_id), button(button), is_down(down)
{}


Event::Event(WindowEvent&& windowev)
    : type(EventType::WINDOW)
    , window(windowev)
{}

Event::Event(DeviceEvent&& deviceev)
    : type(EventType::DEVICE)
    , device(deviceev)
{}

Event::Event(InputEvent&& inputev)
    : type(EventType::INPUT)
    , input(inputev)
{}

Event::Event(RawInputEvent&& rawinev)
    : type(EventType::INPUT_RAW)
    , raw_input(rawinev)
{}
