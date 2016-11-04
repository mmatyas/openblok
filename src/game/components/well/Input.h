#pragma once

#include "system/Event.h"

#include <unordered_map>
#include <vector>


class Well;


namespace WellComponents {

class Input {
public:
    Input();

    /// Update the key states only, but do not activate any game events
    void updateKeystate(const std::vector<InputEvent>&);
    /// Activate game events based on the saved keystate and the current input events
    void handleKeys(Well&, const std::vector<InputEvent>&);

private:
    std::unordered_map<InputType, bool, InputTypeHash> keystates;
    decltype(keystates) previous_keystates;
};

} // namespace WellComponents
