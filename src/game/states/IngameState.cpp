#include "IngameState.h"

#include "game/AppContext.h"
#include "game/layout/gameplay/PlayerArea.h"
#include "substates/Ingame.h"
#include "substates/ingame/FadeInOut.h"
#include "substates/ingame/PlayerSelect.h"
#include "system/Paths.h"
#include "system/Texture.h"
#include <game/states/substates/ingame/Countdown.h>
#include <game/states/substates/ingame/Gameplay.h>


IngameState::IngameState(AppContext& app, GameMode gamemode)
    : gamemode(gamemode)
    , draw_scale(gamemode == GameMode::SP_MARATHON ? 1.0 : 0.8)
    , draw_inverse_scale(1.0 / draw_scale)
    , tex_background(app.gcx().loadTexture(Paths::data() + "gamebg.png"))
{
    updatePositions(app.gcx());
    if (gamemode == GameMode::SP_MARATHON) {
        device_order = {-1};
        states.emplace_back(std::make_unique<SubStates::Ingame::States::Gameplay>(app, *this));
        states.emplace_back(std::make_unique<SubStates::Ingame::States::Countdown>(app));
        states.emplace_back(std::make_unique<SubStates::Ingame::States::FadeIn>([this](){
            states.pop_back();
        }));
    }
    else {
        states.emplace_back(std::make_unique<SubStates::Ingame::States::PlayerSelect>(app));
        states.emplace_back(std::make_unique<SubStates::Ingame::States::FadeIn>([this](){
            states.pop_back();
        }));
    }
}

IngameState::~IngameState() = default;

void IngameState::updatePositions(GraphicsContext& gcx)
{
    if (player_areas.empty())
        return;

    static const int well_padding_x = 5 + Mino::texture_size_px;

    const int center_x = (gcx.screenWidth() * draw_inverse_scale) / 2;
    const int center_y = (gcx.screenHeight() * draw_inverse_scale) / 2;

    const int well_y = center_y - player_areas.at(device_order.front()).height() / 2;
    const int well_full_width = 2 * well_padding_x
        + player_areas.at(device_order.front()).width();
    int available_width = draw_inverse_scale * gcx.screenWidth() / device_order.size();

    int well_x = center_x - (well_full_width * player_areas.size()) / 2;

    for (const DeviceID device_id : device_order) {
        player_areas.at(device_id).setPosition(well_x + well_padding_x, well_y);
        player_areas.at(device_id).setMaxWidth(available_width);
        well_x += well_full_width;
    }
}

void IngameState::update(const std::vector<Event>& events, AppContext& app)
{
    std::map<DeviceID, std::vector<InputEvent>> input_events;
    for (const auto& event : events) {
        switch (event.type) {
            case EventType::WINDOW:
                if (event.window == WindowEvent::RESIZED)
                    updatePositions(app.gcx());
                break;
            case EventType::INPUT:
                input_events[event.input.srcDeviceID()].emplace_back(event.input);
                break;
            default:
                // TODO ?
                break;
        }
    }
    for (auto& ui_pa : player_areas)
        ui_pa.second.well().updateKeystateOnly(input_events[ui_pa.first]);

    states.back()->update(*this, events, app);
}

void IngameState::draw(GraphicsContext& gcx)
{
    drawCommon(gcx);

    const auto original_scale = gcx.getDrawScale();
    gcx.modifyDrawScale(original_scale * draw_scale);

    for(const auto& state : states)
        state->drawPassive(*this, gcx);
    states.back()->drawActive(*this, gcx);

    gcx.modifyDrawScale(original_scale);
}

void IngameState::drawCommon(GraphicsContext& gcx)
{
    tex_background->drawScaled({0, 0, gcx.screenWidth(), gcx.screenHeight()});
}

