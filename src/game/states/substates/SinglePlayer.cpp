#include "SinglePlayer.h"

#include "game/AppContext.h"
#include "game/components/animations/TextPopup.h"
#include "game/states/SinglePlayState.h"
#include "system/AudioContext.h"
#include "system/Color.h"


namespace SubStates {
namespace SinglePlayer {
namespace States {

FadeIn::FadeIn()
    : alpha(std::chrono::milliseconds(500),
            [](double t){ return (1.0 - t) * 0xFF; })
{}

void FadeIn::update(SinglePlayState& parent, const std::vector<Event>&, AppContext&)
{
    alpha.update(Timing::frame_duration);
    if (!alpha.running())
        parent.current_state = std::make_unique<Countdown>();
}

void FadeIn::draw(SinglePlayState&, GraphicsContext& gcx) const
{
    RGBAColor color = 0x00_rgba;
    color.a = alpha.value();
    gcx.drawFilledRect({0, 0, gcx.screenWidth(), gcx.screenHeight()}, color);
}


Countdown::Countdown()
    : timer(std::chrono::seconds(3), [](double){})
{}

void Countdown::update(SinglePlayState& parent, const std::vector<Event>&, AppContext&)
{
    // TODO : display countdown
    //timer.update(Timing::frame_duration);
    //if (!timer.running())
        parent.current_state = std::make_unique<Gameplay>();
}


Gameplay::Gameplay() = default;

void Gameplay::update(SinglePlayState& parent, const std::vector<Event>& events, AppContext& app)
{
    for (const auto& event : events) {
        switch (event.type) {
            case EventType::WINDOW:
                if (event.window == WindowEvent::FOCUS_LOST)
                    parent.current_state = std::make_unique<Pause>(app);
            case EventType::INPUT:
                if (event.input.type() == InputType::GAME_PAUSE && event.input.down())
                    parent.current_state = std::make_unique<Pause>(app);
                break;
            default:
                break;
        }
    }

    parent.ui_well.update(events);
    parent.ui_leftside.update();

    if (parent.texts_need_update) {
        parent.ui_leftside.updateGoalCounter(parent.lineclears_left);
        parent.ui_leftside.updateLevelCounter(parent.current_level);
        parent.ui_rightside.updateScore(parent.current_score);
        parent.texts_need_update = false;
    }

    // newly created popups don't know theit position,
    // that's why this is here, and not in SinglePlayState
    for (auto& popup : parent.textpopups) {
        popup->setInitialPosition(
            parent.ui_leftside.x() - 10 + (parent.ui_leftside.width() - static_cast<int>(popup->width())) / 2.0,
            parent.ui_leftside.y() + parent.ui_leftside.height() * 0.5
        );
        popup->update();
    }

    if (parent.gameover)
        return;

    parent.ui_rightside.updateGametime();
}


Pause::Pause(AppContext& app)
{
    app.audio().pauseAll();
}

void Pause::update(SinglePlayState& parent, const std::vector<Event>& events, AppContext& app)
{
    for (const auto& event : events) {
        if (event.type == EventType::INPUT
            && event.input.type() == InputType::GAME_PAUSE
            && event.input.down()) {
            // exit pause mode
            app.audio().resumeAll();
            parent.current_state = std::make_unique<Countdown>();
        }
    }
}

} // namespace States
} // namespace SinglePlayer
} // namespace SubStates
