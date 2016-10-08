#pragma once

#include "game/GameState.h"
#include "game/components/HoldQueue.h"
#include "game/components/NextQueue.h"
#include "game/components/Well.h"

#include <chrono>
#include <stack>
#include <string>


class SinglePlayState: public GameState {
public:
    SinglePlayState(AppContext&);
    void update(const std::vector<InputEvent>&, AppContext&) final;
    void draw(GraphicsContext&) final;

private:
    using Duration = std::chrono::steady_clock::duration;

    bool paused;
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
    TextureID tex_pause;
    bool texts_need_update;

    HoldQueue piece_holder;
    NextQueue next_pieces;
    Well board;

    const unsigned lineclears_per_level;
    int lineclears_left;
    std::stack<Duration> gravity_levels;
    unsigned current_level;
    unsigned current_score;
    Duration gametime;
    std::string gametime_text;

    void addNextPiece();

    // TODO: create a ui element tree
    struct Layout {
        struct Padding {
            int top;
            int right;
            int bottom;
            int left;
        };
        struct Border {
            const int width = 5;
            Rectangle left;
            Rectangle right;
            Rectangle top;
            Rectangle bottom;
        };

        struct _well {
            Rectangle inner;
            Rectangle outer;

            Border border;
        } well;

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
    void drawWell(GraphicsContext&);
    void drawLeftSidebar(GraphicsContext&);
    void drawRightSidebar(GraphicsContext&);
    void updateGametime(AppContext&);
};
