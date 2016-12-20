#include "Gameplay.h"

#include "MinorStates.h"
#include "game/AppContext.h"
#include "game/components/Piece.h"
#include "game/components/animations/TextPopup.h"
#include "game/states/SinglePlayState.h"
#include "system/AudioContext.h"
#include "system/Localize.h"
#include "system/Music.h"
#include "system/Paths.h"
#include "system/SoundEffect.h"

#include <algorithm>
#include <cmath>


namespace SubStates {
namespace SinglePlayer {
namespace States {

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
    parent.player_area.well().setGravity(gravity_levels.top());

    pending_levelup_msg.stop();

    music->playLoop();
    app.audio().pauseAll();

    registerObservers(parent, app);
}

Gameplay::~Gameplay() = default;

void Gameplay::addNextPiece(SinglePlayState& parent)
{
    parent.player_area.well().addPiece(parent.player_area.nextQueue().next());
    parent.player_area.holdQueue().onNextTurn();
}

void Gameplay::registerObservers(SinglePlayState& parent, AppContext& app)
{
    auto& well = parent.player_area.well();

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
        auto& well = parent.player_area.well();
        auto& hold_queue = parent.player_area.holdQueue();

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
            parent.player_area.well().setGravity(this->gravity_levels.top());
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
    parent.player_area.well().updateAnimationsOnly();

    pending_levelup_msg.update(Timing::frame_duration);
    // remove old animations
    textpopups.erase(std::remove_if(textpopups.begin(), textpopups.end(),
        [](std::unique_ptr<TextPopup>& popup){ return !popup->isActive(); }),
        textpopups.end());

    // newly created popups don't know theit position,
    // that's why this is here, and not in SinglePlayState
    const int center_x = parent.player_area.x() - 10
        + (parent.player_area.wellBox().x - 10 - parent.player_area.x()) / 2;
    for (auto& popup : textpopups) {
        popup->setInitialPosition(
            center_x - static_cast<int>(popup->width()) / 2,
            parent.player_area.y() + parent.player_area.height() / 2
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
                input_events.emplace_back(event.input.type(), event.input.down(), event.input.srcDeviceID());
                break;
            default:
                break;
        }
    }

    parent.player_area.well().updateGameplayOnly(input_events);
    parent.player_area.update();

    if (texts_need_update) {
        parent.player_area.updateGoalCounter(lineclears_left);
        parent.player_area.updateLevelCounter(parent.player_stats.level);
        parent.player_area.updateScore(parent.player_stats.score);
        texts_need_update = false;
    }

    // updating the Well may produce popups, initialized at (0;0),
    // so we update the animations only after that
    updateAnimationsOnly(parent, app);

    parent.player_stats.gametime += Timing::frame_duration;
    parent.player_area.updateGametime(parent.player_stats.gametime);
}

void Gameplay::draw(SinglePlayState& parent, GraphicsContext& gfx) const
{
    parent.player_area.drawActive(gfx);
    for (const auto& popup : textpopups)
        popup->draw();
}

} // namespace States
} // namespace SinglePlayer
} // namespace SubStates
