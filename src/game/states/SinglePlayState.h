#pragma once

#include "game/GameState.h"
#include "game/components/HoldQueue.h"
#include "game/components/NextQueue.h"
#include "game/components/singleplayer/WellBox.h"

#include <stack>
#include <string>


enum ScoreTypes {
    LINE_CLEAR_SINGLE = 1,
    LINE_CLEAR_DOUBLE = 2,
    LINE_CLEAR_TRIPLE = 3,
    LINE_CLEAR_PERFECT = 4,
    SOFTDROP,
    HARDDROP
};


class SinglePlayState: public GameState {
public:
    SinglePlayState(AppContext&);
    ~SinglePlayState();

    void update(const std::vector<InputEvent>&, AppContext&) final;
    void draw(GraphicsContext&) final;

private:
    bool paused;
    bool gameover;
    FontID font_boxtitle;
    FontID font_boxcontent;
    FontID font_big;
    TextureID tex_background;
    TextureID tex_hold;
    TextureID tex_next;
    TextureID tex_goal;
    TextureID tex_goal_counter;
    TextureID tex_level;
    TextureID tex_level_counter;
    TextureID tex_score;
    TextureID tex_score_counter;
    TextureID tex_time_counter;
    bool texts_need_update;

    Layout::WellBox ui_well;

    HoldQueue piece_holder;
    NextQueue next_pieces;

    const unsigned lineclears_per_level;
    int lineclears_left;
    std::stack<Duration> gravity_levels;
    unsigned current_level;
    unsigned current_score;
    Duration gametime;
    std::string gametime_text;
    std::unordered_map<ScoreTypes, unsigned, std::hash<size_t>> score_table;
    ScoreTypes previous_lineclear_type;

    void registerObservers();
    void addNextPiece();


    // TODO: create a ui element tree
    struct Layout {
        struct Padding {
            int top;
            int right;
            int bottom;
            int left;
        };

        struct _sidebars {
            const int text_height = 30;
            const int text_padding = 10;
            const int item_padding = 10;
            const RGBAColor box_color = 0x0A0AFF80_rgba;

            struct {
                Rectangle inner;
                Rectangle outer;
                Padding padding;

                struct {
                    Rectangle level_counter;
                    Rectangle goal_counter;
                } items;
            } left;

            struct {
                Rectangle inner;
                Rectangle outer;
                Padding padding;

                struct {
                    Rectangle time_counter;
                    Rectangle score_counter;
                } items;
            } right;
        } sidebars;
    } ui;
    void drawLeftSidebar(GraphicsContext&);
    void drawRightSidebar(GraphicsContext&);
    void updateGametime(AppContext&);
};
