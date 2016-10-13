#include "SinglePlayState.h"

#include "game/AppContext.h"
#include "game/WellEvent.h"
#include "game/components/Piece.h"
#include "system/Localize.h"

#include <cmath>


SinglePlayState::SinglePlayState(AppContext& app)
    : paused(false)
    , gameover(false)
    , tex_background(app.gcx().loadTexture("data/gamebg.png"))
    , texts_need_update(true)
    , ui_well(app)
    , ui_leftside(app, ui_well.height())
    , ui_rightside(app, ui_well.height())
    , lineclears_per_level(10)
    , lineclears_left(lineclears_per_level)
    , current_level(1)
    , current_score(0)
    , score_table({
            {LINE_CLEAR_SINGLE, 100},
            {LINE_CLEAR_DOUBLE, 200},
            {LINE_CLEAR_TRIPLE, 500},
            {LINE_CLEAR_PERFECT, 800},
            {SOFTDROP, 1},
            {HARDDROP, 2}
        })
    , previous_lineclear_type(LINE_CLEAR_SINGLE)
{
    registerObservers();

    // TODO: consider alternative algorithm
    for (int i = 14; i >= 0; i--) {
        float multiplier = std::pow(0.8 - (i * 0.007), i);
        gravity_levels.push(std::chrono::duration_cast<Duration>(multiplier * std::chrono::seconds(1)));
    }

    addNextPiece();
    ui_well.well().setGravity(gravity_levels.top());
}

SinglePlayState::~SinglePlayState() = default;

void SinglePlayState::registerObservers()
{
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
        }
    });

    ui_well.well().registerObserver(WellEvent::Type::LINE_CLEAR, [this](const WellEvent& event){
        assert(event.type == WellEvent::Type::LINE_CLEAR);
        assert(event.count > 0);
        assert(event.count <= 4);

        this->texts_need_update = true;
        this->lineclears_left -= event.count;


        // increase score
        this->current_score += this->score_table.at(static_cast<ScoreTypes>(event.count))
                               * this->current_level;
        if (event.count == 4 && this->previous_lineclear_type == LINE_CLEAR_PERFECT)
            this->current_score += this->score_table.at(LINE_CLEAR_PERFECT) * 0.5f; // back-to-back bonus

        this->previous_lineclear_type = static_cast<ScoreTypes>(event.count);


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
        }
    });

    ui_well.well().registerObserver(WellEvent::Type::HARDDROPPED, [this](const WellEvent& event){
        assert(event.count < 22);
        this->texts_need_update = true;
        this->current_score += event.count * this->score_table.at(HARDDROP);
    });

    ui_well.well().registerObserver(WellEvent::Type::SOFTDROPPED, [this](const WellEvent&){
        this->texts_need_update = true;
        this->current_score += this->score_table.at(SOFTDROP);
    });

    ui_well.well().registerObserver(WellEvent::Type::GAME_OVER, [this](const WellEvent&){
        gameover = true;
    });
}

void SinglePlayState::addNextPiece()
{
    ui_well.well().addPiece(ui_rightside.nextQueue().next());
    ui_leftside.holdQueue().onNextTurn();
}

void SinglePlayState::update(const std::vector<InputEvent>& inputs, AppContext& app)
{
    for (const auto& input : inputs) {
        if (input.type() == InputType::GAME_PAUSE && input.down()) {
            paused = !paused;
            return;
        }
    }


    // UI
    // TODO: only on screen resize event
    ui_well.setPosition((app.gcx().screenWidth() - ui_well.width()) / 2,
                        (app.gcx().screenHeight() - ui_well.height()) / 2);
    ui_leftside.setPosition(ui_well.x() - ui_leftside.width() - 10, ui_well.y());
    ui_rightside.setPosition(ui_well.x() + ui_well.width() + 10, ui_well.y());


    if (paused)
        return;

    ui_well.update(inputs, app);
    ui_leftside.update();

    if (texts_need_update) {
        ui_leftside.updateGoalCounter(app.gcx(), lineclears_left);
        ui_leftside.updateLevelCounter(app.gcx(), current_level);
        ui_rightside.updateScore(app.gcx(), current_score);
        texts_need_update = false;
    }

    if (gameover)
        return;

    ui_rightside.updateGametime(app.gcx());
}

void SinglePlayState::draw(GraphicsContext& gcx)
{
    gcx.drawTexture(tex_background, {0, 0, gcx.screenWidth(), gcx.screenHeight()});

    ui_well.draw(gcx, paused);
    ui_leftside.draw(gcx);
    ui_rightside.draw(gcx);
}
