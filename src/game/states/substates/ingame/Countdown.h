#pragma once

#include "game/Transition.h"
#include "game/states/substates/Ingame.h"

#include <array>
#include <memory>

class SoundEffect;
class Texture;


namespace SubStates {
namespace Ingame {
namespace States {

class Countdown : public State {
public:
    Countdown(AppContext& app);
    void update(IngameState&, const std::vector<Event>&, AppContext&) final;
    void drawActive(IngameState&, GraphicsContext&) const final;

private:
    uint8_t current_idx;
    Transition<void> timer;

    std::array<std::shared_ptr<SoundEffect>, 3> sfx_countdown;
    std::array<std::unique_ptr<Texture>, 3> tex_countdown;
    std::shared_ptr<SoundEffect> pending_sfx;
};

} // namespace States
} // namespace Ingame
} // namespace SubStates
