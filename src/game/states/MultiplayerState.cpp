#include "MultiplayerState.h"

#include "game/AppContext.h"
#include "substates/Multiplayer.h"
#include "substates/multiplayer/FadeInOut.h"
#include "substates/multiplayer/PlayerSelect.h"
#include "system/Paths.h"
#include "system/Texture.h"

// TODO: move these somewhere else
static const int well_width = 10 * Mino::texture_size_px + 10;
static const int well_height = 20.3 * Mino::texture_size_px + 10;
static const int well_padding_x = Mino::texture_size_px / 2;


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
    if (ui_wells.empty())
        return;
    assert(ui_wells.size() > 1);

    const int well_full_width = well_width + 2 * well_padding_x;
    int well_x = (gcx.screenWidth() - well_full_width * ui_wells.size()) / 2;
    const int well_y = (gcx.screenHeight() - well_height) / 2;

    for (auto& ui_well : ui_wells) {
        ui_well.second.setPosition(well_x, well_y);
        well_x += well_full_width;
    }
}

void MultiplayerState::update(const std::vector<Event>& events, AppContext& app)
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
    for (auto& ui_well : ui_wells)
        ui_well.second.well().updateKeystateOnly(input_events);

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

