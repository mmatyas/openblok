#include "MinorStates.h"

#include "Gameplay.h"
#include "Statistics.h"
#include "game/AppContext.h"
#include "game/states/SinglePlayState.h"
#include "system/AudioContext.h"
#include "system/Color.h"
#include "system/Font.h"
#include "system/Localize.h"
#include "system/SoundEffect.h"
#include "system/Paths.h"
#include "system/Texture.h"

#include <algorithm>


namespace SubStates {
namespace SinglePlayer {
namespace States {

// The state transitions:
//
// FadeIn -> Countdown -> Gameplay -> GameComplete/GameOver -> Statistics -> FadeOut
//               ^           |
//               +-- Pause <-+
//                     |
//                   Quit

FadeIn::FadeIn()
    : alpha(std::chrono::milliseconds(500),
            [](double t){ return (1.0 - t) * 0xFF; })
{}

void FadeIn::update(SinglePlayState& parent, const std::vector<Event>&, AppContext& app)
{
    alpha.update(Timing::frame_duration);
    if (!alpha.running()) {
        parent.states.emplace_back(std::make_unique<Gameplay>(parent, app));
        parent.states.emplace_back(std::make_unique<Countdown>(app));
        parent.states.pop_front(); // pops self!
    }
}

void FadeIn::draw(SinglePlayState&, GraphicsContext& gcx) const
{
    RGBAColor color = 0x00_rgba;
    color.a = alpha.value();
    gcx.drawFilledRect({0, 0, gcx.screenWidth(), gcx.screenHeight()}, color);
}


FadeOut::FadeOut(AppContext& app)
    : alpha(std::chrono::milliseconds(500),
            [](double t){ return t * 0xFF; },
            [&app](){ app.states().pop(); })
{}

void FadeOut::update(SinglePlayState&, const std::vector<Event>&, AppContext&)
{
    alpha.update(Timing::frame_duration);
}

void FadeOut::draw(SinglePlayState&, GraphicsContext& gcx) const
{
    RGBAColor color = 0x00_rgba;
    color.a = alpha.value();
    gcx.drawFilledRect({0, 0, gcx.screenWidth(), gcx.screenHeight()}, color);
}


Countdown::Countdown(AppContext& app)
    : current_idx(0)
    , timer(std::chrono::milliseconds(800), [](double){})
    , sfx_countdown({{
            app.audio().loadSound(Paths::data() + "sfx/countdown3.ogg"),
            app.audio().loadSound(Paths::data() + "sfx/countdown2.ogg"),
            app.audio().loadSound(Paths::data() + "sfx/countdown1.ogg"),
        }})
    , requested_sfx(sfx_countdown.at(0))
{
    auto font_huge = app.gcx().loadFont(Paths::data() + "fonts/helsinki.ttf", 150);
    tex_countdown = {{
        font_huge->renderText("3", 0xEEEEEE_rgb),
        font_huge->renderText("2", 0xEEEEEE_rgb),
        font_huge->renderText("1", 0xEEEEEE_rgb),
    }};
}

void Countdown::update(SinglePlayState& parent, const std::vector<Event>&, AppContext& app)
{
    if (!timer.running()) {
        current_idx++;
        if (current_idx == 3) {
            app.audio().resumeAll();
            assert(parent.states.size() > 1); // there should be a Gameplay state below
            parent.states.pop_back(); // pops self!
            return;
        }
        assert(current_idx < 3);
        requested_sfx = sfx_countdown.at(current_idx);
        this->timer.restart();
    }
    if (requested_sfx) {
        requested_sfx->playOnce();
        requested_sfx.reset();
    }

    timer.update(Timing::frame_duration);
}

void Countdown::draw(SinglePlayState& parent, GraphicsContext&) const
{
    assert(current_idx < 3);
    auto& tex = tex_countdown.at(current_idx);
    tex->drawAt(parent.wellCenterX() - static_cast<int>(tex->width()) / 2,
                parent.wellCenterY() - static_cast<int>(tex->height()) / 2);
}


Pause::Pause(AppContext& app)
    : current_menuitem(0)
{
    app.audio().pauseAll();

    auto font_big = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 45);
    tex_pause = font_big->renderText(tr("PAUSE"), 0xEEEEEE_rgb);

    auto font_smaller = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 30);
    const std::vector<std::string> menuitems = {tr("RESUME"), tr("QUIT")};
    for (const auto& label : menuitems) {
        tex_menuitems.emplace_back(std::array<std::unique_ptr<Texture>, 2>{
            font_smaller->renderText(label, 0xEEEEEE_rgb),
            font_smaller->renderText(label, 0xCE8000_rgb)
        });
    }
}

void Pause::update(SinglePlayState& parent, const std::vector<Event>& events, AppContext& app)
{
    for (const auto& event : events) {
        if (event.type == EventType::INPUT && event.input.down()) {
            switch (event.input.type()) {
                // exit pause mode
                case InputType::GAME_PAUSE:
                    assert(parent.states.size() > 1); // there should be a Gameplay state below
                    parent.states.pop_back();
                    return;
                case InputType::MENU_OK:
                    assert(current_menuitem < tex_menuitems.size());
                    switch (current_menuitem) { // TODO: fix magic numbers
                        case 0: // resume
                            assert(parent.states.size() > 1);
                            parent.states.pop_back();
                            return;
                        case 1: // back to main menu
                            parent.states.emplace_back(std::make_unique<FadeOut>(app));
                            return;
                        default:
                            assert(false);
                    }
                    break;
                case InputType::MENU_UP:
                    if (current_menuitem > 0)
                        current_menuitem--;
                    break;
                case InputType::MENU_DOWN:
                    if (current_menuitem + 1 < tex_menuitems.size())
                        current_menuitem++;
                    break;
                default:
                    break;
            }
        }
    }
}

void Pause::draw(SinglePlayState& parent, GraphicsContext&) const
{
    tex_pause->drawAt(
        parent.wellCenterX() - static_cast<int>(tex_pause->width()) / 2,
        parent.wellCenterY() - static_cast<int>(tex_pause->height()) / 2);

    for (size_t i = 0; i < tex_menuitems.size(); i++) {
        const auto& tex = tex_menuitems.at(i).at(i == current_menuitem ? 1 : 0);
        tex->drawAt(
            parent.wellCenterX() - static_cast<int>(tex->width()) / 2,
            parent.wellCenterY() + 150 - i * - static_cast<int>(tex->height()));
    }
}


GameOver::GameOver(SinglePlayState& parent, AppContext& app)
    : sfx_ongameover(app.audio().loadSound(Paths::data() + "sfx/gameover.ogg"))
    , background_percent(std::chrono::seconds(2),
        [](double t){ return t; },
        [this](){
            this->sfx_ongameover->playOnce();
            this->statistics_delay.restart();
        })
    , statistics_delay(std::chrono::seconds(5), [](double){},
        [&parent, &app](){
            parent.states.emplace_back(std::make_unique<Statistics>(parent, app));
        })
{
    auto font_big = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 45);
    tex_gameover = font_big->renderText(tr("GAME OVER"), 0xEEEEEE_rgb);
    tex_gameover->setAlpha(0x0);

    statistics_delay.stop();
}

void GameOver::update(SinglePlayState& parent, const std::vector<Event>& events, AppContext& app)
{
    parent.states.front()->updateAnimationsOnly(parent, app);
    statistics_delay.update(Timing::frame_duration);
    background_percent.update(Timing::frame_duration);
    if (background_percent.value() > 0.4)
        tex_gameover->setAlpha(std::min<int>(0xFF, (background_percent.value() - 0.4) * 0x1FF));

    for (const auto& event : events) {
        if (event.type == EventType::INPUT && event.input.down()) {
            background_percent.update(background_percent.length());
            tex_gameover->setAlpha(std::min<int>(0xFF, (background_percent.value() - 0.4) * 0x1FF));
            statistics_delay.update(statistics_delay.length());
            return; // stop parsing the rest of the keys
        }
    }
}

void GameOver::draw(SinglePlayState& parent, GraphicsContext& gcx) const
{
    // draw the well with contents, and also any leftover popup texts of Gameplay
    assert(parent.states.size() > 1);
    parent.states.front()->draw(parent, gcx);

    int box_h = parent.ui_well.wellHeight() * background_percent.value();
    gcx.drawFilledRect({
        parent.ui_well.wellX(), parent.ui_well.wellY() + parent.ui_well.wellHeight() - box_h,
        parent.ui_well.wellWidth(), box_h
    }, 0xA0_rgba);

    tex_gameover->drawAt(parent.wellCenterX() - static_cast<int>(tex_gameover->width()) / 2,
                         parent.wellCenterY() - static_cast<int>(tex_gameover->height()) / 2);
}


GameComplete::GameComplete(SinglePlayState& parent, AppContext& app)
    : sfx_onfinish(app.audio().loadSound(Paths::data() + "sfx/finish.ogg"))
    , text_alpha(std::chrono::seconds(1),
        [](double t){ return t * 0xFF; },
        [this](){
            this->statistics_delay.restart();
        })
    , statistics_delay(std::chrono::seconds(4), [](double){},
        [&parent, &app](){
            parent.states.emplace_back(std::make_unique<Statistics>(parent, app));
        })
{
    auto font_big = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 45);
    tex_finish = font_big->renderText(tr("FINISH!"), 0xEEEEEE_rgb);
    tex_finish->setAlpha(0x0);

    statistics_delay.stop();
    sfx_onfinish->playOnce();
}

void GameComplete::update(SinglePlayState& parent, const std::vector<Event>& events, AppContext& app)
{
    parent.states.front()->updateAnimationsOnly(parent, app);
    statistics_delay.update(Timing::frame_duration);
    text_alpha.update(Timing::frame_duration);
    tex_finish->setAlpha(text_alpha.value());

    for (const auto& event : events) {
        if (event.type == EventType::INPUT && event.input.down()) {
            text_alpha.update(text_alpha.length());
            tex_finish->setAlpha(text_alpha.value());
            statistics_delay.update(statistics_delay.length());
            return; // stop parsing the rest of the keys
        }
    }
}

void GameComplete::draw(SinglePlayState& parent, GraphicsContext& gcx) const
{
    // draw the well with contents, and also any leftover popup texts of Gameplay
    assert(parent.states.size() > 1);
    parent.states.front()->draw(parent, gcx);

    tex_finish->drawAt(parent.wellCenterX() - static_cast<int>(tex_finish->width()) / 2,
                       parent.wellCenterY() - static_cast<int>(tex_finish->height()) / 2);
}

} // namespace States
} // namespace SinglePlayer
} // namespace SubStates
