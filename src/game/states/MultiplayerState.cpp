#include "MultiplayerState.h"

#include "game/AppContext.h"
#include "substates/Multiplayer.h"
#include "substates/multiplayer/FadeInOut.h"
#include "substates/multiplayer/PlayerSelect.h"
#include "system/Paths.h"
#include "system/Texture.h"


MultiplayerState::MultiplayerState(AppContext& app)
    : tex_background(app.gcx().loadTexture(Paths::data() + "gamebg.png"))
{
    updatePositions(app.gcx());
    states.emplace_back(std::make_unique<SubStates::Multiplayer::States::PlayerSelect>(app));
    states.emplace_back(std::make_unique<SubStates::Multiplayer::States::FadeIn>([this](){
        states.pop_back();
    }));
}

MultiplayerState::~MultiplayerState() = default;

void MultiplayerState::updatePositions(GraphicsContext& gcx)
{

}

void MultiplayerState::update(const std::vector<Event>& events, AppContext& app)
{
    states.back()->update(*this, events, app);
}

void MultiplayerState::draw(GraphicsContext& gcx)
{
    drawCommon(gcx);
    states.back()->draw(*this, gcx);
}

void MultiplayerState::drawCommon(GraphicsContext& gcx)
{
    tex_background->drawScaled({0, 0, gcx.screenWidth(), gcx.screenHeight()});
}

