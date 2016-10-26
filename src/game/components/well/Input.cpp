#include "Input.h"

#include "game/components/Well.h"


namespace WellComponents {

Input::Input()
{
    keystates[InputType::GAME_MOVE_LEFT] = false;
    keystates[InputType::GAME_MOVE_RIGHT] = false;
    keystates[InputType::GAME_SOFTDROP] = false;
    keystates[InputType::GAME_ROTATE_LEFT] = false;
    keystates[InputType::GAME_ROTATE_RIGHT] = false;
    previous_keystates = keystates;
}

void Input::updateKeystate(const std::vector<InputEvent>& events)
{
    previous_keystates = keystates;

    for (const auto& event : events)
        keystates[event.type()] = event.down();
}

void Input::handleKeys(Well& well, const std::vector<InputEvent>& events)
{
    // keep it true only if down key is still down
    if (keystates.at(InputType::GAME_SOFTDROP) && previous_keystates.at(InputType::GAME_SOFTDROP))
        well.components.gravity.skipNextUpdate();

    if (!well.lock_delay.lockInProgress())
        well.components.tspin.clear();


    // for some events onpress/onrelease handling is better suited
    for (const auto& event : events) {
        // press
        if (event.down()) {
            switch (event.type()) {
            case InputType::GAME_HARDDROP:
                well.hardDrop();
                well.components.gravity.skipNextUpdate();
                break;

            case InputType::GAME_HOLD:
                well.notify(WellEvent(WellEvent::Type::HOLD_REQUESTED));
                well.components.gravity.skipNextUpdate();
                break;

            case InputType::GAME_ROTATE_LEFT:
                well.rotateNow(Well::RotationDirection::COUNTER_CLOCKWISE);
                break;

            case InputType::GAME_ROTATE_RIGHT:
                well.rotateNow(Well::RotationDirection::CLOCKWISE);
                break;

            default:
                break;
            }
        }
        // release
        else {
            switch (event.type()) {
            case InputType::GAME_MOVE_LEFT:
            case InputType::GAME_MOVE_RIGHT:
                well.components.das.reset();
                break;
            default:
                break;
            }
        }
    }


    well.components.das.update();
    if (well.components.das.movementAllowed()) {
        if (keystates.at(InputType::GAME_MOVE_LEFT) != keystates.at(InputType::GAME_MOVE_RIGHT)) {
            if (keystates.at(InputType::GAME_MOVE_LEFT))
                well.moveLeftNow();
            else
                well.moveRightNow();

            well.components.das.onHorizontalMove();
        }
    }

    well.softdrop_timer -= Timing::frame_duration;
    if (keystates.at(InputType::GAME_SOFTDROP) && well.softdrop_timer <= Duration::zero()) {
        well.moveDownNow();
        well.components.gravity.skipNextUpdate();
        well.softdrop_timer = well.softdrop_delay;
        if (well.active_piece && !well.lock_delay.lockInProgress())
            well.notify(WellEvent(WellEvent::Type::SOFTDROPPED));
    }
}

} // namespace WellComponents
