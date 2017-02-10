#pragma once

#include "game/Transition.h"
#include "game/states/substates/Ingame.h"
#include "system/Color.h"

#include <memory>
#include <unordered_map>

class Texture;


namespace SubStates {
namespace Ingame {
namespace States {

class Statistics : public State {
public:
    Statistics(IngameState&, AppContext&);
    void update(IngameState&, const std::vector<Event>&, AppContext&) final;
    void drawPassive(IngameState&, GraphicsContext&) const final;

private:
    std::unique_ptr<Texture> tex_title;
    std::vector<std::unique_ptr<Texture>> labels;
    std::unordered_map<DeviceID, std::vector<std::unique_ptr<Texture>>> scores;

    Transition<double> fadein_percent;
    Transition<uint8_t> displayed_item_count;

    RGBAColor bg_color;

    void drawBackground(IngameState&, GraphicsContext&) const;
    void drawItems(IngameState&) const;
};

} // namespace States
} // namespace Ingame
} // namespace SubStates
