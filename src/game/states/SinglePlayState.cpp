#include "SinglePlayState.h"

#include "game/AppContext.h"
#include "system/Texture.h"


SinglePlayState::SinglePlayState(AppContext& app)
    : tex_background(app.gcx().loadTexture(DATADIR + "gamebg.png"))
    , ui_well(app)
    , ui_leftside(app, ui_well.height())
    , ui_rightside(app, ui_well.height())
{
    updatePositions(app.gcx());
    states.emplace_back(std::make_unique<SubStates::SinglePlayer::States::FadeIn>());
}

SinglePlayState::~SinglePlayState() = default;

void SinglePlayState::updatePositions(GraphicsContext& gcx)
{
    ui_well.setPosition((gcx.screenWidth() - ui_well.width()) / 2,
                        (gcx.screenHeight() - ui_well.height()) / 2);
    ui_leftside.setPosition(ui_well.x() - ui_leftside.width() - 10, ui_well.y());
    ui_rightside.setPosition(ui_well.x() + ui_well.width() + 10, ui_well.y());
}

void SinglePlayState::update(const std::vector<Event>& events, AppContext& app)
{
    for (const auto& event : events) {
        if (event.type == EventType::WINDOW
            && event.window == WindowEvent::RESIZED) {
            updatePositions(app.gcx());
        }
    }

    ui_well.update(events, states.back()->type());
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

    ui_well.draw(gcx, states.back()->type());
    ui_leftside.draw(gcx);
    ui_rightside.draw(gcx);
}
