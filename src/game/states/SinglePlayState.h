#pragma once

#include "game/GameState.h"
#include "game/PlayerStatistics.h"
#include "game/layout/gameplay/PlayerArea.h"

#include <list>
#include <memory>

namespace SubStates { namespace SinglePlayer {
    class State;
    namespace States {
        class GameOver;
        class Gameplay;
        class Statistics;
    }
} }


class SinglePlayState: public GameState {
public:
    SinglePlayState(AppContext&);
    ~SinglePlayState();

    void update(const std::vector<Event>&, AppContext&) final;
    void draw(GraphicsContext&) final;

    std::list<std::unique_ptr<SubStates::SinglePlayer::State>> states;

    int wellCenterX() const { return player_area.wellCenterX(); };
    int wellCenterY() const { return player_area.wellCenterY(); };

private:
    PlayerStatistics player_stats;
    std::unique_ptr<Texture> tex_background;

    Layout::PlayerArea player_area;

    void updatePositions(GraphicsContext&);
    void drawCommon(GraphicsContext&);

    friend class SubStates::SinglePlayer::States::GameOver;
    friend class SubStates::SinglePlayer::States::Gameplay;
    friend class SubStates::SinglePlayer::States::Statistics;
};
