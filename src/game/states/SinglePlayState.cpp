#include "SinglePlayState.h"

#include "game/AppContext.h"
#include "game/WellEvent.h"
#include "game/components/Piece.h"
#include "game/components/animations/TextPopup.h"
#include "system/AudioContext.h"
#include "system/Localize.h"
#include "system/Music.h"
#include "system/SoundEffect.h"

#include <algorithm>
#include <cmath>


SinglePlayState::SinglePlayState(AppContext& app)
    : gameover(false)
    , tex_background(app.gcx().loadTexture("data/gamebg.png"))
    , music(app.audio().loadMusic("data/music/gameplay.ogg"))
    , sfx_onhold(app.audio().loadSound("data/sfx/hold.ogg"))
    , sfx_onlevelup(app.audio().loadSound("data/sfx/levelup.ogg"))
    , sfx_onlineclear({
            app.audio().loadSound("data/sfx/lineclear1.ogg"),
            app.audio().loadSound("data/sfx/lineclear2.ogg"),
            app.audio().loadSound("data/sfx/lineclear3.ogg"),
            app.audio().loadSound("data/sfx/lineclear4.ogg"),
        })
    , sfx_onlock(app.audio().loadSound("data/sfx/lock.ogg"))
    , sfx_onrotate(app.audio().loadSound("data/sfx/rotate.ogg"))
    , texts_need_update(true)
    , ui_well(app)
    , ui_leftside(app, ui_well.height())
    , ui_rightside(app, ui_well.height())
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
    , pending_levelup_msg(std::chrono::milliseconds(500), [](double){}, [this](){
            this->textpopups.emplace_back(std::make_unique<TextPopup>(tr("LEVEL UP!"), font_popuptext));
        })
    , current_state(std::make_unique<SubStates::SinglePlayer::States::FadeIn>())
{
    font_popuptext = app.gcx().loadFont("data/fonts/PTS76F.ttf", 34);
    pending_levelup_msg.stop();

    registerObservers();

    // TODO: consider alternative algorithm
    for (int i = 14; i >= 0; i--) {
        float multiplier = std::pow(0.8 - (i * 0.007), i);
        gravity_levels.push(std::chrono::duration_cast<Duration>(multiplier * std::chrono::seconds(1)));
    }

    addNextPiece();
    ui_well.well().setGravity(gravity_levels.top());

    updatePositions(app.gcx());
    music->playLoop();
}

SinglePlayState::~SinglePlayState() = default;

void SinglePlayState::registerObservers()
{
    ui_well.well().registerObserver(WellEvent::Type::PIECE_LOCKED, [this](const WellEvent&){
        this->sfx_onlock->playOnce();
    });

    ui_well.well().registerObserver(WellEvent::Type::PIECE_ROTATED, [this](const WellEvent&){
        this->sfx_onrotate->playOnce();
    });

    ui_well.well().registerObserver(WellEvent::Type::NEXT_REQUESTED, [this](const WellEvent&){
        this->addNextPiece();
    });

    ui_well.well().registerObserver(WellEvent::Type::HOLD_REQUESTED, [this](const WellEvent&){
        auto& well = this->ui_well.well();
        auto& hold_queue = this->ui_leftside.holdQueue();

        hold_queue.onSwapRequested();
        if (hold_queue.swapAllowed()) {
            auto type = well.activePiece()->type();
            well.deletePiece();
            if (hold_queue.isEmpty()) {
                hold_queue.swapWithEmpty(type);
                this->addNextPiece();
            }
            else
                well.addPiece(hold_queue.swapWith(type));

            this->sfx_onhold->playOnce();
        }
    });

    ui_well.well().registerObserver(WellEvent::Type::LINE_CLEAR_ANIMATION_START, [this](const WellEvent& event){
        assert(event.type == WellEvent::Type::LINE_CLEAR_ANIMATION_START);
        assert(event.lineclear.count > 0);
        assert(event.lineclear.count <= 4);
        this->sfx_onlineclear.at(event.lineclear.count - 1)->playOnce();
    });

    ui_well.well().registerObserver(WellEvent::Type::LINE_CLEAR, [this](const WellEvent& event){
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
            this->ui_well.well().setGravity(this->gravity_levels.top());
            this->lineclears_left += lineclears_per_level;
            this->current_level++;
            this->sfx_onlevelup->playOnce();
            // produce delayed popup if there are other popups already
            this->pending_levelup_msg.restart();
            if (this->textpopups.empty())
                this->pending_levelup_msg.update(this->pending_levelup_msg.length());
        }
    });

    ui_well.well().registerObserver(WellEvent::Type::MINI_TSPIN_DETECTED, [this](const WellEvent&){
        this->texts_need_update = true;
        this->current_score += this->score_table.at(ScoreType::MINI_TSPIN);
        this->textpopups.emplace_back(std::make_unique<TextPopup>(
            this->score_name.at(ScoreType::MINI_TSPIN),
            this->font_popuptext));
    });

    ui_well.well().registerObserver(WellEvent::Type::TSPIN_DETECTED, [this](const WellEvent&){
        this->texts_need_update = true;
        this->current_score += this->score_table.at(ScoreType::TSPIN);
        this->textpopups.emplace_back(std::make_unique<TextPopup>(
            this->score_name.at(ScoreType::TSPIN),
            this->font_popuptext));
    });

    ui_well.well().registerObserver(WellEvent::Type::HARDDROPPED, [this](const WellEvent& event){
        assert(event.harddrop.count < 22);
        this->texts_need_update = true;
        this->current_score += event.harddrop.count * this->score_table.at(ScoreType::HARDDROP);
    });

    ui_well.well().registerObserver(WellEvent::Type::SOFTDROPPED, [this](const WellEvent&){
        this->texts_need_update = true;
        this->current_score += this->score_table.at(ScoreType::SOFTDROP);
    });

    ui_well.well().registerObserver(WellEvent::Type::GAME_OVER, [this](const WellEvent&){
        gameover = true;
        music->fadeOut(std::chrono::seconds(1));
    });
}

void SinglePlayState::addNextPiece()
{
    ui_well.well().addPiece(ui_rightside.nextQueue().next());
    ui_leftside.holdQueue().onNextTurn();
}

void SinglePlayState::updatePositions(GraphicsContext& gcx)
{
    ui_well.setPosition((gcx.screenWidth() - ui_well.width()) / 2,
                        (gcx.screenHeight() - ui_well.height()) / 2);
    ui_leftside.setPosition(ui_well.x() - ui_leftside.width() - 10, ui_well.y());
    ui_rightside.setPosition(ui_well.x() + ui_well.width() + 10, ui_well.y());
}

void SinglePlayState::update(const std::vector<Event>& events, AppContext& app)
{
    for (const auto& event : events) {
        if (event.type == EventType::WINDOW
            && event.window == WindowEvent::RESIZED) {
            updatePositions(app.gcx());
        }
    }

    pending_levelup_msg.update(Timing::frame_duration);
    // remove old animations
    textpopups.erase(std::remove_if(textpopups.begin(), textpopups.end(),
        [](std::unique_ptr<TextPopup>& popup){ return !popup->isActive(); }),
        textpopups.end());


    current_state->update(*this, events, app);
}

void SinglePlayState::draw(GraphicsContext& gcx)
{
    drawCommon(gcx);
    current_state->draw(*this, gcx);
}

void SinglePlayState::drawCommon(GraphicsContext& gcx)
{
    tex_background->drawScaled({0, 0, gcx.screenWidth(), gcx.screenHeight()});

    ui_well.draw(gcx, current_state->type());
    ui_leftside.draw(gcx);
    ui_rightside.draw(gcx);

    for (const auto& popup : textpopups)
        popup->draw();
}
