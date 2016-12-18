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
static const int well_padding_x = 5 + Mino::texture_size_px;


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

    const float scale = 0.8;
    const float inverse_scale = 1.f / scale;

    const int well_full_width = well_width + 2 * well_padding_x;
    int well_x = well_padding_x + (gcx.screenWidth() * inverse_scale - well_full_width * ui_wells.size()) / 2;
    const int well_y = 40 + (gcx.screenHeight() * inverse_scale - well_height) / 2;

    assert(ui_wells.size() == ui_topbars.size());
    for (const DeviceID device_id : device_order) {
        auto& topbar = ui_topbars.at(device_id);
        topbar.setPosition(well_x, well_y - 10 - topbar.height());

        auto& bottombar = ui_bottombars.at(device_id);
        bottombar.setPosition(well_x, well_y + well_height + 10);

        ui_wells.at(device_id).setPosition(well_x, well_y);
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

