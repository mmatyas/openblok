#include "SinglePlayer.h"

#include "game/AppContext.h"
#include "game/components/Piece.h"
#include "game/components/animations/TextPopup.h"
#include "game/states/SinglePlayState.h"
#include "game/util/DurationToString.h"
#include "system/AudioContext.h"
#include "system/Color.h"
#include "system/Font.h"
#include "system/Localize.h"
#include "system/Music.h"
#include "system/SoundEffect.h"
#include "system/Paths.h"
#include "system/Texture.h"

#include <algorithm>
#include <cmath>


namespace SubStates {
namespace SinglePlayer {
namespace States {

// The state transitions:
//
// FadeIn -> Countdown -> Gameplay -> Finish
//               ^           |
//               +-- Pause <-+
//

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
        font_huge->renderText(tr("3"), 0xEEEEEE_rgb),
        font_huge->renderText(tr("2"), 0xEEEEEE_rgb),
        font_huge->renderText(tr("1"), 0xEEEEEE_rgb),
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
{
    app.audio().pauseAll();

    auto font_big = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 45);
    tex = font_big->renderText(tr("PAUSE"), 0xEEEEEE_rgb);
}

void Pause::update(SinglePlayState& parent, const std::vector<Event>& events, AppContext&)
{
    for (const auto& event : events) {
        if (event.type == EventType::INPUT
            && event.input.type() == InputType::GAME_PAUSE
            && event.input.down()) {
            // exit pause mode
            assert(parent.states.size() > 1); // there should be a Gameplay state below
            parent.states.pop_back();
        }
    }
}

void Pause::draw(SinglePlayState& parent, GraphicsContext&) const
{
    tex->drawAt(parent.wellCenterX() - static_cast<int>(tex->width()) / 2,
                parent.wellCenterY() - static_cast<int>(tex->height()) / 2);
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

void GameOver::update(SinglePlayState&, const std::vector<Event>&, AppContext&)
{
    statistics_delay.update(Timing::frame_duration);
    background_percent.update(Timing::frame_duration);
    if (background_percent.value() > 0.4)
        tex_gameover->setAlpha(std::min<int>(0xFF, (background_percent.value() - 0.4) * 0x1FF));
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

void GameComplete::update(SinglePlayState& parent, const std::vector<Event>&, AppContext& app)
{
    parent.states.front()->updateAnimationsOnly(parent, app);
    statistics_delay.update(Timing::frame_duration);
    text_alpha.update(Timing::frame_duration);
    tex_finish->setAlpha(text_alpha.value());
}

void GameComplete::draw(SinglePlayState& parent, GraphicsContext& gcx) const
{
    // draw the well with contents, and also any leftover popup texts of Gameplay
    assert(parent.states.size() > 1);
    parent.states.front()->draw(parent, gcx);

    tex_finish->drawAt(parent.wellCenterX() - static_cast<int>(tex_finish->width()) / 2,
                       parent.wellCenterY() - static_cast<int>(tex_finish->height()) / 2);
}


Statistics::Statistics(SinglePlayState& parent, AppContext& app)
    : background_percent(
        std::chrono::seconds(1),
        [](double t){ return t; },
        [this](){ this->title_alpha.restart(); })
    , title_alpha(
        std::chrono::milliseconds(700),
        [](double t){ return t * 0xFF; },
        [this](){ this->displayed_item_count.restart(); })
    , displayed_item_count(
        std::chrono::seconds(5),
        [this](double t){ return t * this->score_texs.size(); })
    , column_width(420)
    , column_padding(35)
    , title_padding_bottom(40)
{
    // NOTE: the magic numbers in this class were found
    // by trial and error, to look good on all kinds of aspect ratios

    displayed_item_count.stop();
    title_alpha.stop();

    auto color = 0xEEEEEE_rgb;
    auto font_huge = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 64);

    tex_title = font_huge->renderText(tr("STATISTICS"), color);
    tex_title->setAlpha(0x0);

    auto font = app.gcx().loadFont(Paths::data() + "fonts/PTS55F.ttf", 26);
    auto stats = parent.player_stats;

    score_texs.emplace_back(font->renderText(tr("Total Lines"), color),
                            font->renderText(std::to_string(stats.total_cleared_lines), color));
    score_texs.emplace_back(font->renderText(tr("Singles"), color),
                            font->renderText(std::to_string(stats.event_count[ScoreType::CLEAR_SINGLE]), color));
    score_texs.emplace_back(font->renderText(tr("Doubles"), color),
                            font->renderText(std::to_string(stats.event_count[ScoreType::CLEAR_DOUBLE]), color));
    score_texs.emplace_back(font->renderText(tr("Triples"), color),
                            font->renderText(std::to_string(stats.event_count[ScoreType::CLEAR_TRIPLE]), color));
    score_texs.emplace_back(font->renderText(tr("Perfects"), color),
                            font->renderText(std::to_string(stats.event_count[ScoreType::CLEAR_PERFECT]), color));

    score_texs.emplace_back(font->renderText(tr("Back-to-Back"), color),
                            font->renderText(std::to_string(stats.back_to_back_count), color));
    score_texs.emplace_back(font->renderText(tr("Longest Back-to-Back"),
                            color), font->renderText(std::to_string(stats.back_to_back_longest), color));

    score_texs.emplace_back(font->renderText(tr("T-Spin Minis"), color),
                            font->renderText(std::to_string(stats.event_count[ScoreType::MINI_TSPIN]), color));
    score_texs.emplace_back(font->renderText(tr("T-Spin Mini Singles"), color),
                            font->renderText(std::to_string(stats.event_count[ScoreType::CLEAR_MINI_TSPIN_SINGLE]), color));
    score_texs.emplace_back(font->renderText(tr("T-Spins"), color),
                            font->renderText(std::to_string(stats.event_count[ScoreType::TSPIN]), color));
    score_texs.emplace_back(font->renderText(tr("T-Spin Singles"), color),
                            font->renderText(std::to_string(stats.event_count[ScoreType::CLEAR_TSPIN_SINGLE]), color));
    score_texs.emplace_back(font->renderText(tr("T-Spin Doubles"), color),
                            font->renderText(std::to_string(stats.event_count[ScoreType::CLEAR_TSPIN_DOUBLE]), color));
    score_texs.emplace_back(font->renderText(tr("T-Spin Triples"), color),
                            font->renderText(std::to_string(stats.event_count[ScoreType::CLEAR_TSPIN_TRIPLE]), color));
    score_texs.emplace_back(font->renderText(tr("Duration"), color),
                            font->renderText(Timing::toString(stats.gametime), color));

    auto color_highlight = 0xFEC500_rgb;
    auto font_highlight = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 26);
    score_texs.emplace_back(font_highlight->renderText(tr("Level"), color_highlight),
                            font_highlight->renderText(std::to_string(stats.level), color_highlight));
    score_texs.emplace_back(font_highlight->renderText(tr("Final Score"), color_highlight),
                            font_highlight->renderText(std::to_string(stats.score), color_highlight));
}

void Statistics::update(SinglePlayState&, const std::vector<Event>& events, AppContext& app)
{
    background_percent.update(Timing::frame_duration);
    title_alpha.update(Timing::frame_duration);
    displayed_item_count.update(Timing::frame_duration);

    tex_title->setAlpha(title_alpha.value());

    if (state_transition_alpha) {
        state_transition_alpha->update(Timing::frame_duration);
        return;
    }

    for (const auto& event : events) {
        switch (event.type) {
            case EventType::INPUT:
                state_transition_alpha = std::make_unique<Transition<uint8_t>>(
                    std::chrono::milliseconds(500),
                    [](double t){ return t * 0xFF; },
                    [&app](){ app.states().pop(); }
                );
                break;
            default:
                break;
        }
    }
}

void Statistics::draw(SinglePlayState& parent, GraphicsContext& gcx) const
{
    // draw gameover shadow and well content
    auto it = parent.states.rbegin();
    it++;
    assert(it != parent.states.rend());
    (*it)->draw(parent, gcx);

    drawBackground(parent, gcx);
    drawItems(parent);

    if (state_transition_alpha) {
        RGBAColor color = 0xFF_rgba;
        color.a = state_transition_alpha->value();
        gcx.drawFilledRect({0, 0, gcx.screenWidth(), gcx.screenHeight()}, color);
    }
}

void Statistics::drawBackground(SinglePlayState& parent, GraphicsContext& gcx) const
{
    const int height = 10 + parent.ui_well.wellHeight() * background_percent.value();
    const int width = gcx.screenWidth() * 0.92;
    const int half_height = height / 2;
    const int half_width = width / 2;
    gcx.drawFilledRect({
        parent.wellCenterX() - half_width,
        parent.wellCenterY() - half_height,
        width, height},
        0x2030FF_rgb);
}

void Statistics::drawItems(SinglePlayState& parent) const
{
    static const int item_height = score_texs.at(0).first->height() * 1.2;
    static const unsigned row_count_per_column = score_texs.size() / 2;

    if (!background_percent.running()) {
        // draw title
        int row_pos_x = parent.wellCenterX() - column_width;
        int first_row_pos_y = parent.wellCenterY() - ((score_texs.size() / 4) - 1) * item_height;

        tex_title->drawAt(row_pos_x, first_row_pos_y - tex_title->height() - title_padding_bottom);

        // draw statistics items
        if (!title_alpha.running()) {
            int row_pos_y = first_row_pos_y;
            unsigned current_row = 0;
            unsigned idx = 0;
            while (idx < displayed_item_count.value()) {
                score_texs.at(idx).first->drawAt(row_pos_x, row_pos_y);
                auto& val_tex = score_texs.at(idx).second;
                val_tex->drawAt(row_pos_x + column_width - column_padding - val_tex->width(), row_pos_y);

                idx++;
                current_row++;
                row_pos_y += item_height;
                if (current_row >= row_count_per_column) {
                    current_row = 0;
                    row_pos_y = first_row_pos_y;
                    row_pos_x += column_width + column_padding;
                }
            }
        }
    }
}


Gameplay::Gameplay(SinglePlayState& parent, AppContext& app)
    : music(app.audio().loadMusic(Paths::data() + "music/gameplay.ogg"))
    , sfx_onhold(app.audio().loadSound(Paths::data() + "sfx/hold.ogg"))
    , sfx_onlevelup(app.audio().loadSound(Paths::data() + "sfx/levelup.ogg"))
    , sfx_onlineclear({{
            app.audio().loadSound(Paths::data() + "sfx/lineclear1.ogg"),
            app.audio().loadSound(Paths::data() + "sfx/lineclear2.ogg"),
            app.audio().loadSound(Paths::data() + "sfx/lineclear3.ogg"),
            app.audio().loadSound(Paths::data() + "sfx/lineclear4.ogg"),
        }})
    , sfx_onlock(app.audio().loadSound(Paths::data() + "sfx/lock.ogg"))
    , sfx_onrotate(app.audio().loadSound(Paths::data() + "sfx/rotate.ogg"))
    , font_popuptext(app.gcx().loadFont(Paths::data() + "fonts/PTS76F.ttf", 34))
    , texts_need_update(true)
    , pending_levelup_msg(std::chrono::milliseconds(500), [](double){}, [this](){
            this->textpopups.emplace_back(std::make_unique<TextPopup>(tr("LEVEL UP!"), font_popuptext));
        })
    , lineclears_per_level(10)
    , lineclears_left(lineclears_per_level)
    , previous_lineclear_type(ScoreType::CLEAR_SINGLE)
    , back2back_length(0)
{
    // TODO: consider alternative algorithm
    for (int i = 14; i >= 0; i--) {
        float multiplier = std::pow(0.8 - (i * 0.007), i);
        gravity_levels.push(std::chrono::duration_cast<Duration>(multiplier * std::chrono::seconds(1)));
    }
    parent.ui_well.well().setGravity(gravity_levels.top());

    pending_levelup_msg.stop();

    music->playLoop();
    app.audio().pauseAll();

    registerObservers(parent, app);
}

void Gameplay::addNextPiece(SinglePlayState& parent)
{
    parent.ui_well.well().addPiece(parent.ui_rightside.nextQueue().next());
    parent.ui_leftside.holdQueue().onNextTurn();
}

void Gameplay::registerObservers(SinglePlayState& parent, AppContext& app)
{
    auto& well = parent.ui_well.well();

    well.registerObserver(WellEvent::Type::PIECE_LOCKED, [this](const WellEvent&){
        this->sfx_onlock->playOnce();
    });

    well.registerObserver(WellEvent::Type::PIECE_ROTATED, [this](const WellEvent&){
        this->sfx_onrotate->playOnce();
    });

    well.registerObserver(WellEvent::Type::NEXT_REQUESTED, [this, &parent](const WellEvent&){
        // if the game is still running
        if (!this->gravity_levels.empty())
            this->addNextPiece(parent);
    });

    well.registerObserver(WellEvent::Type::HOLD_REQUESTED, [this, &parent](const WellEvent&){
        auto& well = parent.ui_well.well();
        auto& hold_queue = parent.ui_leftside.holdQueue();

        hold_queue.onSwapRequested();
        if (hold_queue.swapAllowed()) {
            auto type = well.activePiece()->type();
            well.deletePiece();
            if (hold_queue.isEmpty()) {
                hold_queue.swapWithEmpty(type);
                this->addNextPiece(parent);
            }
            else
                well.addPiece(hold_queue.swapWith(type));

            this->sfx_onhold->playOnce();
        }
    });

    well.registerObserver(WellEvent::Type::LINE_CLEAR_ANIMATION_START, [this](const WellEvent& event){
        assert(event.type == WellEvent::Type::LINE_CLEAR_ANIMATION_START);
        assert(event.lineclear.count > 0);
        assert(event.lineclear.count <= 4);
        this->sfx_onlineclear.at(event.lineclear.count - 1)->playOnce();
    });

    well.registerObserver(WellEvent::Type::LINE_CLEAR, [this, &parent, &app](const WellEvent& event){
        assert(event.type == WellEvent::Type::LINE_CLEAR);
        assert(event.lineclear.count > 0);
        assert(event.lineclear.count <= 4);

        this->texts_need_update = true;
        this->lineclears_left -= event.lineclear.count;
        parent.player_stats.total_cleared_lines += event.lineclear.count;


        // increase score
        auto score_type = ScoreTable::lineclearType(event.lineclear);
        unsigned score = ScoreTable::value(score_type);
        std::string popup_text = ScoreTable::name(score_type);
        parent.player_stats.event_count[score_type]++;

        if (ScoreTable::canContinueBackToBack(this->previous_lineclear_type, score_type)) {
            score *= ScoreTable::back2backMultiplier();
            popup_text = ScoreTable::back2backName() + "\n" + popup_text;
            back2back_length++;
            parent.player_stats.back_to_back_count++;
            parent.player_stats.back_to_back_longest = std::max(parent.player_stats.back_to_back_longest, back2back_length);
        }
        else
            back2back_length = 0;

        parent.player_stats.score += score * parent.player_stats.level;
        this->previous_lineclear_type = score_type;

        if (score_type != ScoreType::CLEAR_SINGLE) {
            this->textpopups.emplace_back(
                std::make_unique<TextPopup>(popup_text, this->font_popuptext));
        }


        // increase gravity level
        if (this->lineclears_left <= 0) {
            this->gravity_levels.pop();
            if (this->gravity_levels.empty()) {
                this->lineclears_left = 0;
                music->fadeOut(std::chrono::seconds(1));
                parent.states.emplace_back(std::make_unique<GameComplete>(parent, app));
                return;
            }
            parent.ui_well.well().setGravity(this->gravity_levels.top());
            this->lineclears_left += this->lineclears_per_level;
            parent.player_stats.level++;
            sfx_onlevelup->playOnce();

            // produce delayed popup if there are other popups already
            pending_levelup_msg.restart();
            if (textpopups.empty())
                pending_levelup_msg.update(this->pending_levelup_msg.length());
        }
    });

    well.registerObserver(WellEvent::Type::MINI_TSPIN_DETECTED, [this, &parent](const WellEvent&){
        this->texts_need_update = true;
        parent.player_stats.score += ScoreTable::value(ScoreType::MINI_TSPIN);
        parent.player_stats.event_count[ScoreType::MINI_TSPIN]++;
        this->textpopups.emplace_back(std::make_unique<TextPopup>(
            ScoreTable::name(ScoreType::MINI_TSPIN),
            this->font_popuptext));
    });

    well.registerObserver(WellEvent::Type::TSPIN_DETECTED, [this, &parent](const WellEvent&){
        this->texts_need_update = true;
        parent.player_stats.score += ScoreTable::value(ScoreType::TSPIN);
        parent.player_stats.event_count[ScoreType::TSPIN]++;
        this->textpopups.emplace_back(std::make_unique<TextPopup>(
            ScoreTable::name(ScoreType::TSPIN),
            this->font_popuptext));
    });

    well.registerObserver(WellEvent::Type::HARDDROPPED, [this, &parent](const WellEvent& event){
        assert(event.harddrop.count < 22);
        this->texts_need_update = true;
        parent.player_stats.score += event.harddrop.count * ScoreTable::value(ScoreType::HARDDROP);
    });

    well.registerObserver(WellEvent::Type::SOFTDROPPED, [this, &parent](const WellEvent&){
        this->texts_need_update = true;
        parent.player_stats.score += ScoreTable::value(ScoreType::SOFTDROP);
    });

    well.registerObserver(WellEvent::Type::GAME_OVER, [this, &parent, &app](const WellEvent&){
        music->fadeOut(std::chrono::seconds(1));
        parent.states.emplace_back(std::make_unique<GameOver>(parent, app));
    });
}

void Gameplay::updateAnimationsOnly(SinglePlayState& parent, AppContext&)
{
    pending_levelup_msg.update(Timing::frame_duration);
    // remove old animations
    textpopups.erase(std::remove_if(textpopups.begin(), textpopups.end(),
        [](std::unique_ptr<TextPopup>& popup){ return !popup->isActive(); }),
        textpopups.end());

    // newly created popups don't know theit position,
    // that's why this is here, and not in SinglePlayState
    for (auto& popup : textpopups) {
        popup->setInitialPosition(
            parent.ui_leftside.x() - 10 + (parent.ui_leftside.width() - static_cast<int>(popup->width())) / 2.0,
            parent.ui_leftside.y() + parent.ui_leftside.height() * 0.5
        );
        popup->update();
    }
}

void Gameplay::update(SinglePlayState& parent, const std::vector<Event>& events, AppContext& app)
{
    std::vector<InputEvent> input_events;
    for (const auto& event : events) {
        switch (event.type) {
            case EventType::WINDOW:
                if (event.window == WindowEvent::FOCUS_LOST) {
                    parent.states.emplace_back(std::make_unique<Countdown>(app));
                    parent.states.emplace_back(std::make_unique<Pause>(app));
                    return;
                }
                break;
            case EventType::INPUT:
                if (event.input.type() == InputType::GAME_PAUSE && event.input.down()) {
                    parent.states.emplace_back(std::make_unique<Countdown>(app));
                    parent.states.emplace_back(std::make_unique<Pause>(app));
                    return;
                }
                input_events.emplace_back(event.input.type(), event.input.down());
                break;
            default:
                break;
        }
    }

    parent.ui_well.well().updateGameplayOnly(input_events);
    parent.ui_leftside.update();

    if (texts_need_update) {
        parent.ui_leftside.updateGoalCounter(lineclears_left);
        parent.ui_leftside.updateLevelCounter(parent.player_stats.level);
        parent.ui_rightside.updateScore(parent.player_stats.score);
        texts_need_update = false;
    }

    // updating the Well may produce popups, initialized at (0;0),
    // so we update the animations only after that
    updateAnimationsOnly(parent, app);

    parent.player_stats.gametime += Timing::frame_duration;
    parent.ui_rightside.updateGametime(parent.player_stats.gametime);
}

void Gameplay::draw(SinglePlayState& parent, GraphicsContext& gfx) const
{
    parent.ui_well.drawContent(gfx);
    for (const auto& popup : textpopups)
        popup->draw();
}

} // namespace States
} // namespace SinglePlayer
} // namespace SubStates
