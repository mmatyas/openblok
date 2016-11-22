#pragma once

#include "game/Transition.h"
#include "game/states/substates/SinglePlayer.h"

#include <memory>

class Texture;


namespace SubStates {
namespace SinglePlayer {
namespace States {

class Statistics : public State {
public:
    Statistics(SinglePlayState&, AppContext&);
    StateType type() const { return StateType::STATISTICS; }
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    std::unique_ptr<Transition<uint8_t>> state_transition_alpha;
    std::unique_ptr<Texture> tex_title;
    std::vector<std::pair<std::unique_ptr<Texture>, std::unique_ptr<Texture>>> score_texs;

    Transition<double> background_percent;
    Transition<uint8_t> title_alpha;
    Transition<uint8_t> displayed_item_count;

    const int column_width;
    const int column_padding;
    const int title_padding_bottom;

    void drawBackground(SinglePlayState&, GraphicsContext&) const;
    void drawItems(SinglePlayState&) const;
};

} // namespace States
} // namespace SinglePlayer
} // namespace SubStates
