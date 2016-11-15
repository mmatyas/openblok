#pragma once

#include "game/GameState.h"
#include "game/PlayerStatistics.h"
#include "game/layout/singleplayer/LeftSidebarBox.h"
#include "game/layout/singleplayer/RightSidebarBox.h"
#include "game/layout/singleplayer/WellBox.h"
#include "substates/SinglePlayer.h"

#include <list>
#include <memory>


class SinglePlayState: public GameState {
public:
    SinglePlayState(AppContext&);
    ~SinglePlayState();

    void update(const std::vector<Event>&, AppContext&) final;
    void draw(GraphicsContext&) final;

    std::list<std::unique_ptr<SubStates::SinglePlayer::State>> states;

    int wellCenterX() const { return ui_well.x() + ui_well.width() / 2; };
    int wellCenterY() const { return ui_well.y() + ui_well.height() / 2; };

private:
    PlayerStatistics player_stats;
    std::unique_ptr<Texture> tex_background;

    Layout::WellBox ui_well;
    Layout::LeftSidebarBox ui_leftside;
    Layout::RightSidebarBox ui_rightside;

    void updatePositions(GraphicsContext&);
    void drawCommon(GraphicsContext&);

    friend class SubStates::SinglePlayer::States::GameOver;
    friend class SubStates::SinglePlayer::States::Gameplay;
    friend class SubStates::SinglePlayer::States::Statistics;
};
