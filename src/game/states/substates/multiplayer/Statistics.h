#pragma once

#include "game/Transition.h"
#include "game/states/substates/Multiplayer.h"

#include <memory>
#include <unordered_map>

class Texture;


namespace SubStates {
namespace Multiplayer {
namespace States {

class Statistics : public State {
public:
    Statistics(MultiplayerState&, AppContext&);
    void update(MultiplayerState&, const std::vector<Event>&, AppContext&) final;
    void drawPassive(MultiplayerState&, GraphicsContext&) const final;

private:
    std::unique_ptr<Texture> tex_title;
    std::vector<std::unique_ptr<Texture>> labels;
    std::unordered_map<DeviceID, std::vector<std::unique_ptr<Texture>>> scores;

    Transition<double> fadein_percent;
    Transition<uint8_t> displayed_item_count;

    void drawBackground(MultiplayerState&, GraphicsContext&) const;
    void drawItems(MultiplayerState&) const;
};

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
