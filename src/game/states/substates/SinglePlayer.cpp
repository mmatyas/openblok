#include "SinglePlayer.h"

#include "game/AppContext.h"
#include "game/components/Piece.h"
#include "game/components/animations/TextPopup.h"
#include "game/states/SinglePlayState.h"
#include "system/AudioContext.h"
#include "system/Color.h"
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
{
    timer.stop();
}

void Countdown::update(SinglePlayState& parent, const std::vector<Event>&, AppContext& app)
{
    timer.update(Timing::frame_duration);

    if (!timer.running()) {
        if (current_idx < 3)
            sfx_countdown.at(current_idx)->playOnce();
        else {
            app.audio().resumeAll();
            assert(parent.states.size() > 1); // there should be a Gameplay state below
            parent.states.pop_back(); // pops self!
            return;
        }

        this->current_idx++;
        this->timer.restart();
    }
}


Pause::Pause(AppContext& app)
{
    app.audio().pauseAll();
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


Gameplay::Gameplay(SinglePlayState& parent, AppContext& app)
    : gameover(false)
    , music(app.audio().loadMusic(Paths::data() + "music/gameplay.ogg"))
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
    , current_level(1)
    , current_score(0)
    , score_table({
            {ScoreType::CLEAR_SINGLE, 100},
            {ScoreType::CLEAR_DOUBLE, 200},
            {ScoreType::CLEAR_TRIPLE, 500},
            {ScoreType::CLEAR_PERFECT, 800},
            {ScoreType::MINI_TSPIN, 100},
            {ScoreType::CLEAR_MINI_TSPIN_SINGLE, 200},
            {ScoreType::TSPIN, 400},
            {ScoreType::CLEAR_TSPIN_SINGLE, 800},
            {ScoreType::CLEAR_TSPIN_DOUBLE, 1200},
            {ScoreType::CLEAR_TSPIN_TRIPLE, 1600},
            {ScoreType::SOFTDROP, 1},
            {ScoreType::HARDDROP, 2}
        })
    , score_name({
            {ScoreType::CLEAR_SINGLE, tr("SINGLE")},
            {ScoreType::CLEAR_DOUBLE, tr("DOUBLE")},
            {ScoreType::CLEAR_TRIPLE, tr("TRIPLE")},
            {ScoreType::CLEAR_PERFECT, tr("PERFECT!")},
            {ScoreType::MINI_TSPIN, tr("T-SPIN MINI")},
            {ScoreType::CLEAR_MINI_TSPIN_SINGLE, tr("T-MINI SINGLE")},
            {ScoreType::TSPIN, tr("T-SPIN")},
            {ScoreType::CLEAR_TSPIN_SINGLE, tr("T-SPIN SINGLE")},
            {ScoreType::CLEAR_TSPIN_DOUBLE, tr("T-SPIN DOUBLE")},
            {ScoreType::CLEAR_TSPIN_TRIPLE, tr("T-SPIN TRIPLE")}
        })
    , previous_lineclear_type(ScoreType::CLEAR_SINGLE)
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

    registerObservers(parent);
}

void Gameplay::addNextPiece(SinglePlayState& parent)
{
    parent.ui_well.well().addPiece(parent.ui_rightside.nextQueue().next());
    parent.ui_leftside.holdQueue().onNextTurn();
}

void Gameplay::registerObservers(SinglePlayState& parent)
{
    auto& well = parent.ui_well.well();

    well.registerObserver(WellEvent::Type::PIECE_LOCKED, [this](const WellEvent&){
        this->sfx_onlock->playOnce();
    });

    well.registerObserver(WellEvent::Type::PIECE_ROTATED, [this](const WellEvent&){
        this->sfx_onrotate->playOnce();
    });

    well.registerObserver(WellEvent::Type::NEXT_REQUESTED, [this, &parent](const WellEvent&){
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

    well.registerObserver(WellEvent::Type::LINE_CLEAR, [this, &parent](const WellEvent& event){
        assert(event.type == WellEvent::Type::LINE_CLEAR);
        assert(event.lineclear.count > 0);
        assert(event.lineclear.count <= 4);

        this->texts_need_update = true;
        this->lineclears_left -= event.lineclear.count;


        // increase score
        ScoreType score_type = ScoreType::SOFTDROP; // a dummy value
        switch(event.lineclear.type) {
            case LineClearType::NORMAL:
                switch(event.lineclear.count) {
                    case 1: score_type = ScoreType::CLEAR_SINGLE; break;
                    case 2: score_type = ScoreType::CLEAR_DOUBLE; break;
                    case 3: score_type = ScoreType::CLEAR_TRIPLE; break;
                    case 4: score_type = ScoreType::CLEAR_PERFECT; break;
                }
                break;
            case LineClearType::MINI_TSPIN:
                if (event.lineclear.count == 1)
                    score_type = ScoreType::CLEAR_MINI_TSPIN_SINGLE;
                else
                    score_type = ScoreType::CLEAR_DOUBLE; // corner case: next to the wall
                break;
            case LineClearType::TSPIN:
                switch(event.lineclear.count) {
                    case 1: score_type = ScoreType::CLEAR_TSPIN_SINGLE; break;
                    case 2: score_type = ScoreType::CLEAR_TSPIN_DOUBLE; break;
                    case 3: score_type = ScoreType::CLEAR_TSPIN_TRIPLE; break;
                }
                break;
        }
        assert(score_type != ScoreType::SOFTDROP);
        unsigned score = this->score_table.at(score_type);
        std::string popup_text = this->score_name.at(score_type);

        // if the previous and the current clear types are both in this set, add Back-to-Back bonus
        static const std::set<ScoreType> b2b_allowed = {
            ScoreType::CLEAR_PERFECT,
            ScoreType::CLEAR_MINI_TSPIN_SINGLE,
            ScoreType::CLEAR_TSPIN_SINGLE,
            ScoreType::CLEAR_TSPIN_DOUBLE,
            ScoreType::CLEAR_TSPIN_TRIPLE,
        };

        if (b2b_allowed.count(score_type) && b2b_allowed.count(this->previous_lineclear_type)) {
            score *= 1.5;
            popup_text = tr("BACK-TO-BACK\n") + popup_text;
        }

        this->current_score += score * this->current_level;
        this->previous_lineclear_type = score_type;

        if (score_type != ScoreType::CLEAR_SINGLE) {
            this->textpopups.emplace_back(
                std::make_unique<TextPopup>(popup_text, this->font_popuptext));
        }


        // increase gravity level
        if (this->lineclears_left <= 0) {
            if (this->gravity_levels.size() <= 1) {
               this->lineclears_left = 0;
                return;
            }
            this->gravity_levels.pop();
            parent.ui_well.well().setGravity(this->gravity_levels.top());
            this->lineclears_left += this->lineclears_per_level;
            this->current_level++;
            sfx_onlevelup->playOnce();
            // produce delayed popup if there are other popups already
            pending_levelup_msg.restart();
            if (textpopups.empty())
                pending_levelup_msg.update(this->pending_levelup_msg.length());
        }
    });

    well.registerObserver(WellEvent::Type::MINI_TSPIN_DETECTED, [this](const WellEvent&){
        this->texts_need_update = true;
        this->current_score += this->score_table.at(ScoreType::MINI_TSPIN);
        this->textpopups.emplace_back(std::make_unique<TextPopup>(
            this->score_name.at(ScoreType::MINI_TSPIN),
            this->font_popuptext));
    });

    well.registerObserver(WellEvent::Type::TSPIN_DETECTED, [this](const WellEvent&){
        this->texts_need_update = true;
        this->current_score += this->score_table.at(ScoreType::TSPIN);
        this->textpopups.emplace_back(std::make_unique<TextPopup>(
            this->score_name.at(ScoreType::TSPIN),
            this->font_popuptext));
    });

    well.registerObserver(WellEvent::Type::HARDDROPPED, [this](const WellEvent& event){
        assert(event.harddrop.count < 22);
        this->texts_need_update = true;
        this->current_score += event.harddrop.count * this->score_table.at(ScoreType::HARDDROP);
    });

    well.registerObserver(WellEvent::Type::SOFTDROPPED, [this](const WellEvent&){
        this->texts_need_update = true;
        this->current_score += this->score_table.at(ScoreType::SOFTDROP);
    });

    well.registerObserver(WellEvent::Type::GAME_OVER, [this](const WellEvent&){
        gameover = true;
        music->fadeOut(std::chrono::seconds(1));
    });
}

void Gameplay::update(SinglePlayState& parent, const std::vector<Event>& events, AppContext& app)
{
    for (const auto& event : events) {
        switch (event.type) {
            case EventType::WINDOW:
                if (event.window == WindowEvent::FOCUS_LOST) {
                    parent.states.emplace_back(std::make_unique<Countdown>(app));
                    parent.states.emplace_back(std::make_unique<Pause>(app));
                    return;
                }
            case EventType::INPUT:
                if (event.input.type() == InputType::GAME_PAUSE && event.input.down()) {
                    parent.states.emplace_back(std::make_unique<Countdown>(app));
                    parent.states.emplace_back(std::make_unique<Pause>(app));
                    return;
                }
                break;
            default:
                break;
        }
    }

    parent.ui_leftside.update();

    if (texts_need_update) {
        parent.ui_leftside.updateGoalCounter(lineclears_left);
        parent.ui_leftside.updateLevelCounter(current_level);
        parent.ui_rightside.updateScore(current_score);
        texts_need_update = false;
    }

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

    // TODO: new state
    if (gameover)
        return;

    parent.ui_rightside.updateGametime();
}

void Gameplay::draw(SinglePlayState&, GraphicsContext&) const
{
    for (const auto& popup : textpopups)
        popup->draw();
}

} // namespace States
} // namespace SinglePlayer
} // namespace SubStates
