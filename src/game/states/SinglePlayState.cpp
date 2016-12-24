#include "SinglePlayState.h"

#include "game/AppContext.h"
#include "system/Paths.h"
#include "system/Texture.h"
#include "substates/singleplayer/MinorStates.h"


SinglePlayState::SinglePlayState(AppContext& app)
    : tex_background(app.gcx().loadTexture(Paths::data() + "gamebg.png"))
    , player_area(app, false)
{
    updatePositions(app.gcx());
    states.emplace_back(std::make_unique<SubStates::SinglePlayer::States::FadeIn>());
}

SinglePlayState::~SinglePlayState() = default;

void SinglePlayState::updatePositions(GraphicsContext& gcx)
{
    player_area.setPosition((gcx.screenWidth() - player_area.width()) / 2,
                            (gcx.screenHeight() - player_area.height()) / 2);
}

void SinglePlayState::update(const std::vector<Event>& events, AppContext& app)
{
    std::vector<InputEvent> input_events;
    for (const auto& event : events) {
        switch (event.type) {
            case EventType::WINDOW:
                if (event.window == WindowEvent::RESIZED)
                    updatePositions(app.gcx());
                break;
            case EventType::INPUT:
                input_events.emplace_back(event.input.type(), event.input.down(), event.input.srcDeviceID());
                break;
            default:
                // TODO ?
                break;
        }
    }
    player_area.well().updateKeystateOnly(input_events);
    states.back()->update(*this, events, app);
}

void SinglePlayState::draw(GraphicsContext& gcx)
{
    drawCommon(gcx);
    states.back()->draw(*this, gcx);
}

void SinglePlayState::drawCommon(GraphicsContext& gcx)
{
    tex_background->drawScaled({0, 0, gcx.screenWidth(), gcx.screenHeight()});
    player_area.drawPassive(gcx);
}
