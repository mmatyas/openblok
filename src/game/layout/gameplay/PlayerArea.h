#pragma once

#include "GarbageGauge.h"
#include "WellContainer.h"
#include "game/Timing.h"
#include "game/Theme.h"
#include "game/layout/Box.h"
#include "game/components/HoldQueue.h"
#include "game/components/NextQueue.h"
#include "system/Color.h"
#include "system/SoundEffect.h"

class AppContext;
class Font;
class GraphicsContext;
class SoundEffect;


namespace Layout {
class PlayerArea : public Layout::Box {
public:
    PlayerArea(AppContext& app, bool draw_gauge, GameplayTheme theme_cfg);
    virtual ~PlayerArea() {}

    void update();
    void setPosition(int x, int y) override;
    void setMaxWidth(unsigned);
    void drawActive(GraphicsContext&) const;
    void drawPassive(GraphicsContext&) const;

    void setLevelCounter(unsigned);
    void setScore(unsigned);
    void setGametime(Duration);
    void setGoalCounter(unsigned);
    void setGarbageCount(unsigned);

    void startGameOver();
    void startGameFinish();

    void enableGameOverSFX(bool);

    HoldQueue& holdQueue() { return hold_queue; }
    NextQueue& nextQueue() { return next_queue; }
    unsigned queuedGarbageLines() const { return garbage_gauge.lineCount(); }

    Well& well() { return ui_well.well(); };
    ::Rectangle wellBox() const { return wellbox; }
    int wellCenterX() const { return wellBox().x + wellBox().w / 2; }
    int wellCenterY() const { return wellBox().y + wellBox().h / 2; }

protected:
    ::Rectangle wellbox;
    WellContainer ui_well;

    GameplayTheme theme_cfg;
    const RGBAColor labelcolor_normal;
    const RGBAColor labelcolor_highlight;

    const RGBAColor box_color = 0x0A0AFF80_rgba;
    static constexpr int inner_padding = 10;
    static constexpr int sidebar_width = 5 * Mino::texture_size_px;
    static constexpr int topbar_height = 4 * Mino::texture_size_px;
    static constexpr int bottombar_height = 30 + 2 * 5;

    std::shared_ptr<Font> font_content;
    std::shared_ptr<Font> font_content_highlight;

    std::unique_ptr<Texture> tex_hold;
    HoldQueue hold_queue;

    std::unique_ptr<Texture> tex_next;
    NextQueue next_queue;

    const bool draw_gauge;
    GarbageGauge garbage_gauge;

    ::Rectangle rect_level;
    std::unique_ptr<Texture> tex_level;
    std::unique_ptr<Texture> tex_level_counter_wide;
    std::unique_ptr<Texture> tex_level_counter_narrow;

    ::Rectangle rect_score;
    std::unique_ptr<Texture> tex_score;
    std::unique_ptr<Texture> tex_score_counter;

    ::Rectangle rect_goal;
    std::unique_ptr<Texture> tex_goal;
    std::unique_ptr<Texture> tex_goal_counter;

    std::string gametime_text;
    ::Rectangle rect_time;
    std::unique_ptr<Texture> tex_time_counter;

    void calcWellBox();

    std::function<void()> layout_fn;
    void calcWideLayout();
    void calcNarrowLayout();

    std::function<void(GraphicsContext&)> draw_fn_active;
    std::function<void(GraphicsContext&)> draw_fn_passive;
    void drawWideActive(GraphicsContext&) const;
    void drawWidePassive(GraphicsContext&) const;
    void drawNarrowActive(GraphicsContext&) const;
    void drawNarrowPassive(GraphicsContext&) const;

    struct GameEndVars {
        bool gameoversfx_enabled;
        std::unique_ptr<Texture> tex_gameover;
        std::unique_ptr<Texture> tex_finish;
        std::shared_ptr<SoundEffect> sfx_ongameover;
        std::shared_ptr<SoundEffect> sfx_onfinish;
        std::shared_ptr<SoundEffect> sfx_onanimend;
        Transition<double> anim_percent;

        GameEndVars(AppContext&);
    };
    GameEndVars game_end;
    bool enabled_gameoversfx;
    std::function<void()> special_update;
    std::function<void(GraphicsContext&)> special_draw;
};
} // namespace Layout
