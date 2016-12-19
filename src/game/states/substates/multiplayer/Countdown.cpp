#include "Countdown.h"

#include "system/Color.h"
#include "game/AppContext.h"
#include "game/states/MultiplayerState.h"
#include "system/AudioContext.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Paths.h"
#include "system/SoundEffect.h"


namespace SubStates {
namespace Multiplayer {
namespace States {

Countdown::Countdown(AppContext& app)
    : current_idx(0)
    , timer(std::chrono::milliseconds(800), [](double){})
    , sfx_countdown({{
            app.audio().loadSound(Paths::data() + "sfx/countdown3.ogg"),
            app.audio().loadSound(Paths::data() + "sfx/countdown2.ogg"),
            app.audio().loadSound(Paths::data() + "sfx/countdown1.ogg"),
        }})
    , requested_sfx(sfx_countdown.at(0))
{
    auto font_huge = app.gcx().loadFont(Paths::data() + "fonts/helsinki.ttf", 150);
    tex_countdown = {{
        font_huge->renderText("3", 0xEEEEEE_rgb),
        font_huge->renderText("2", 0xEEEEEE_rgb),
        font_huge->renderText("1", 0xEEEEEE_rgb),
    }};
}

void Countdown::update(MultiplayerState& parent, const std::vector<Event>&, AppContext& app)
{
    if (!timer.running()) {
        current_idx++;
        if (current_idx == 3) {
            app.audio().resumeAll();
            assert(parent.states.size() > 1); // there should be a Gameplay state below
            parent.states.pop_back(); // pops self!
            return;
        }
        assert(current_idx < 3);
        requested_sfx = sfx_countdown.at(current_idx);
        this->timer.restart();
    }
    if (requested_sfx) {
        requested_sfx->playOnce();
        requested_sfx.reset();
    }

    timer.update(Timing::frame_duration);
}

void Countdown::drawActive(MultiplayerState& parent, GraphicsContext&) const
{
    assert(current_idx < 3);
    const auto& tex = tex_countdown.at(current_idx);
    for (const auto& ui_well : parent.ui_wells) {
        const int center_x = ui_well.second.wellX() + ui_well.second.wellWidth() / 2;
        const int center_y = ui_well.second.wellY() + ui_well.second.wellHeight() / 2;
        tex->drawAt(center_x - tex->width() / 2, center_y - tex->height() / 2);
    }
}

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
