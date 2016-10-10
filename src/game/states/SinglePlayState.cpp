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
    , texts_need_update(false)
    , next_pieces(4)
    , lineclears_per_level(10)
    , lineclears_left(lineclears_per_level)
    , current_level(1)
    , current_score(0)
    , gametime(Duration::zero())
    , gametime_text("00:00")
{
    board.registerObserver(WellEvent::Type::NEXT_REQUESTED, [this](const WellEvent&){
        this->addNextPiece();
    });

    board.registerObserver(WellEvent::Type::HOLD_REQUESTED, [this](const WellEvent&){
        this->piece_holder.onSwapRequested();
        if (this->piece_holder.swapAllowed()) {
            auto type = this->board.activePiece()->type();
            this->board.deletePiece();
            if (this->piece_holder.isEmpty()) {
                this->piece_holder.swapWithEmpty(type);
                this->addNextPiece();
            }
            else
                this->board.addPiece(this->piece_holder.swapWith(type));
        }
    });

    board.registerObserver(WellEvent::Type::LINE_CLEAR, [this](const WellEvent& event){
        assert(event.type == WellEvent::Type::LINE_CLEAR);
        assert(event.count > 0);
        assert(event.count <= 4);

        this->texts_need_update = true;
        this->lineclears_left -= event.count;

        // increase gravity level
        if (this->lineclears_left <= 0) {
            if (this->gravity_levels.size() <= 1) {
                this->lineclears_left = 0;
                return;
            }
            this->gravity_levels.pop();
            this->board.setGravity(this->gravity_levels.top());
            this->lineclears_left += lineclears_per_level;
            this->current_level++;
        }
    });

    board.registerObserver(WellEvent::Type::GAME_OVER, [this](const WellEvent&){
        gameover = true;
    });


    // TODO: consider alternative algorithm
    for (int i = 14; i >= 0; i--) {
        float multiplier = std::pow(0.8 - (i * 0.007), i);
        gravity_levels.push(std::chrono::duration_cast<Duration>(multiplier * std::chrono::seconds(1)));
    }

    addNextPiece();
    board.setGravity(gravity_levels.top());


    // UI

    font_boxtitle = app.gcx().loadFont(app.fontman().find("Dosis", "light"), 28);
    font_boxcontent = app.gcx().loadFont(app.fontman().find("Dosis", "bold"), 30);
    font_big = app.gcx().loadFont(app.fontman().find("Dosis", "bold"), 50);


    tex_hold = app.gcx().renderText(tr("HOLD"), font_boxtitle, 0xEEEEEE_rgb);
    tex_next = app.gcx().renderText(tr("NEXT"), font_boxtitle, 0xEEEEEE_rgb);
    tex_goal = app.gcx().renderText(tr("GOAL"), font_boxtitle, 0xEEEEEE_rgb);
    tex_level = app.gcx().renderText(tr("LEVEL"), font_boxtitle, 0xEEEEEE_rgb);
    tex_goal_counter = app.gcx().renderText(std::to_string(lineclears_left), font_boxcontent, 0xEEEEEE_rgb);
    tex_level_counter = app.gcx().renderText(std::to_string(current_level), font_boxcontent, 0xEEEEEE_rgb);
    tex_score = app.gcx().renderText(tr("SCORE"), font_boxtitle, 0xEEEEEE_rgb);
    tex_score_counter = app.gcx().renderText(std::to_string(current_score), font_boxcontent, 0xEEEEEE_rgb);
    tex_time_counter = app.gcx().renderText(gametime_text, font_boxcontent, 0xEEEEEE_rgb);
    tex_pause = app.gcx().renderText(tr("PAUSE"), font_big, 0xEEEEEE_rgb);


    ui.well.inner.w = 10 * Mino::texture_size_px;
    ui.well.inner.h = 20 * Mino::texture_size_px;
    ui.well.outer.w = ui.well.inner.w + ui.well.border.width * 2;
    ui.well.outer.h = ui.well.inner.h + ui.well.border.width * 2;

    ui.sidebars.left.inner.w = 5 * Mino::texture_size_px;
    ui.sidebars.left.inner.h = ui.well.outer.h;
    ui.sidebars.left.padding = {0, 10, 0, 0};
    ui.sidebars.left.outer.w = ui.sidebars.left.inner.w
                             + ui.sidebars.left.padding.left + ui.sidebars.left.padding.right;
    ui.sidebars.left.outer.h = ui.sidebars.left.inner.h
                             + ui.sidebars.left.padding.top + ui.sidebars.left.padding.bottom;

    ui.sidebars.right.inner.w = 5 * Mino::texture_size_px;
    ui.sidebars.right.inner.h = ui.well.outer.h;
    ui.sidebars.right.padding = {0, 0, 0, 10};
    ui.sidebars.right.outer.w = ui.sidebars.right.inner.w
                              + ui.sidebars.right.padding.left + ui.sidebars.left.padding.right;
    ui.sidebars.right.outer.h = ui.sidebars.right.inner.h
                              + ui.sidebars.right.padding.top + ui.sidebars.left.padding.bottom;
}

SinglePlayState::~SinglePlayState() = default;

void SinglePlayState::addNextPiece()
{
    board.addPiece(next_pieces.next());
    piece_holder.onNextTurn();
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
    ui.well.inner.x = app.gcx().screenWidth() / 2 - 5 * Mino::texture_size_px;
    ui.well.inner.y = (app.gcx().screenHeight() - ui.well.inner.h) / 2 + ui.well.border.width;
    ui.well.outer.x = ui.well.inner.x - ui.well.border.width;
    ui.well.outer.y = ui.well.inner.y - ui.well.border.width;
    ui.well.border.left = {
        ui.well.outer.x, ui.well.inner.y,
        ui.well.border.width, ui.well.inner.h};
    ui.well.border.right = {
        ui.well.inner.x + ui.well.inner.w, ui.well.inner.y,
        ui.well.border.width, ui.well.inner.h};
    ui.well.border.top = {
        ui.well.outer.x, ui.well.outer.y,
        ui.well.outer.w, ui.well.border.width};
    ui.well.border.bottom = {
        ui.well.outer.x, ui.well.inner.y + ui.well.inner.h,
        ui.well.outer.w, ui.well.border.width};

    ui.sidebars.left.outer.x = ui.well.outer.x - ui.sidebars.left.outer.w;
    ui.sidebars.left.inner.x = ui.sidebars.left.outer.x + ui.sidebars.left.padding.left;
    ui.sidebars.left.outer.y = ui.well.outer.y;
    ui.sidebars.left.inner.y = ui.sidebars.left.outer.y + ui.sidebars.left.padding.top;
    ui.sidebars.left.items.goal_counter = {
        ui.sidebars.left.inner.x,
        ui.sidebars.left.inner.y + ui.sidebars.left.inner.h
        - ui.sidebars.text_height - ui.sidebars.text_padding * 2,
        ui.sidebars.left.inner.w, ui.sidebars.text_height + ui.sidebars.text_padding * 2};
    ui.sidebars.left.items.level_counter = {
        ui.sidebars.left.inner.x,
        ui.sidebars.left.items.goal_counter.y
        - ui.sidebars.text_padding - ui.sidebars.text_height - ui.sidebars.text_padding
        - ui.sidebars.item_padding
        - ui.sidebars.left.items.goal_counter.h,
        ui.sidebars.left.inner.w, ui.sidebars.text_height + ui.sidebars.text_padding * 2};

    ui.sidebars.right.outer.x = ui.well.outer.x + ui.well.outer.w;
    ui.sidebars.right.inner.x = ui.sidebars.right.outer.x + ui.sidebars.right.padding.left;
    ui.sidebars.right.outer.y = ui.well.outer.y;
    ui.sidebars.right.inner.y = ui.sidebars.right.outer.y + ui.sidebars.right.padding.top;
    ui.sidebars.right.items.score_counter = {
        ui.sidebars.right.inner.x,
        ui.sidebars.right.inner.y + ui.sidebars.right.inner.h
        - ui.sidebars.text_height - ui.sidebars.text_padding * 2,
        ui.sidebars.right.inner.w, ui.sidebars.text_height + ui.sidebars.text_padding * 2};
    ui.sidebars.right.items.time_counter = {
        ui.sidebars.right.inner.x,
        ui.sidebars.right.items.score_counter.y
        - ui.sidebars.text_padding - ui.sidebars.text_height - ui.sidebars.text_padding
        - ui.sidebars.item_padding
        - ui.sidebars.right.items.score_counter.h,
        ui.sidebars.right.inner.w, ui.sidebars.text_height + ui.sidebars.text_padding * 2};


    if (paused)
        return;

    board.update(inputs, app);
    piece_holder.update();

    if (texts_need_update) {
        app.gcx().renderText(tex_goal_counter, std::to_string(lineclears_left),
                             font_boxcontent, 0xEEEEEE_rgb);
        app.gcx().renderText(tex_level_counter, std::to_string(current_level),
                             font_boxcontent, 0xEEEEEE_rgb);
        texts_need_update = false;
    }

    if (gameover)
        return;

    updateGametime(app);
}

void SinglePlayState::updateGametime(AppContext& app)
{
    gametime += GameState::frame_duration;

    unsigned minutes = std::chrono::duration_cast<std::chrono::minutes>(gametime).count();
    unsigned seconds = std::chrono::duration_cast<std::chrono::seconds>(gametime).count() % 60;
    std::string newstr;
    if (minutes < 10)
        newstr += "0";
    newstr += std::to_string(minutes);
    newstr += ":";
    if (seconds < 10)
        newstr += "0";
    newstr += std::to_string(seconds);

    // render text
    if (newstr != gametime_text) {
        gametime_text = newstr;
        app.gcx().renderText(tex_time_counter, gametime_text, font_boxcontent, 0xEEEEEE_rgb);
    }
}

void SinglePlayState::drawWell(GraphicsContext& gcx)
{
    board.drawBackground(gcx, ui.well.inner.x, ui.well.inner.y);
    if (paused) {
        gcx.drawTexture(tex_pause,
                        ui.well.inner.x + (ui.well.inner.w - gcx.textureWidth(tex_pause)) / 2,
                        ui.well.inner.y + (ui.well.inner.h - gcx.textureHeight(tex_pause)) / 2);
    }
    else {
        board.drawContent(gcx, ui.well.inner.x, ui.well.inner.y);
    }

    static const auto boardborder_color = 0x1A3A8A_rgb;
    gcx.drawFilledRect(ui.well.border.left, boardborder_color);
    gcx.drawFilledRect(ui.well.border.right, boardborder_color);
    gcx.drawFilledRect(ui.well.border.top, boardborder_color);
    gcx.drawFilledRect(ui.well.border.bottom, boardborder_color);
}

void SinglePlayState::drawLeftSidebar(GraphicsContext& gcx)
{
    // hold queue
    gcx.drawTexture(tex_hold, ui.sidebars.left.inner.x, ui.sidebars.left.inner.y);
    piece_holder.draw(gcx, ui.sidebars.left.inner.x,
                      ui.sidebars.left.inner.y + ui.sidebars.text_height + ui.sidebars.text_padding);

    // goal
    gcx.drawFilledRect(ui.sidebars.left.items.goal_counter, ui.sidebars.box_color);
    gcx.drawTexture(tex_goal_counter,
                    ui.sidebars.left.items.goal_counter.x
                    + (ui.sidebars.left.items.goal_counter.w - gcx.textureWidth(tex_goal_counter)) / 2,
                    ui.sidebars.left.items.goal_counter.y + 5);
    gcx.drawTexture(tex_goal, ui.sidebars.left.items.goal_counter.x,
                    ui.sidebars.left.items.goal_counter.y
                    - ui.sidebars.text_padding - ui.sidebars.text_height);

    // level
    gcx.drawFilledRect(ui.sidebars.left.items.level_counter, ui.sidebars.box_color);
    gcx.drawTexture(tex_level_counter,
                    ui.sidebars.left.items.level_counter.x
                    + (ui.sidebars.left.items.level_counter.w - gcx.textureWidth(tex_level_counter)) / 2,
                    ui.sidebars.left.items.level_counter.y + 5);
    gcx.drawTexture(tex_level, ui.sidebars.left.items.level_counter.x,
                    ui.sidebars.left.items.level_counter.y
                    - ui.sidebars.text_padding - ui.sidebars.text_height);
}

void SinglePlayState::drawRightSidebar(GraphicsContext& gcx)
{
    // next queue
    gcx.drawTexture(tex_next,
                    ui.sidebars.right.inner.x + ui.sidebars.right.inner.w - gcx.textureWidth(tex_next),
                    ui.sidebars.right.inner.y);
    next_pieces.draw(gcx, ui.sidebars.right.inner.x,
                     ui.sidebars.right.inner.y + ui.sidebars.text_height + ui.sidebars.text_padding);

    // score
    gcx.drawFilledRect(ui.sidebars.right.items.score_counter, ui.sidebars.box_color);
    gcx.drawTexture(tex_score_counter,
                    ui.sidebars.right.items.score_counter.x
                    + (ui.sidebars.right.items.score_counter.w - gcx.textureWidth(tex_score_counter)) / 2,
                    ui.sidebars.right.items.score_counter.y + 5);
    gcx.drawTexture(tex_score,
                    ui.sidebars.right.items.score_counter.x
                    + ui.sidebars.right.inner.w - gcx.textureWidth(tex_score),
                    ui.sidebars.right.items.score_counter.y
                    - ui.sidebars.text_padding - ui.sidebars.text_height);

    // time
    gcx.drawFilledRect(ui.sidebars.right.items.time_counter, ui.sidebars.box_color);
    gcx.drawTexture(tex_time_counter,
                    ui.sidebars.right.items.time_counter.x
                    + (ui.sidebars.right.items.time_counter.w - gcx.textureWidth(tex_time_counter)) / 2,
                    ui.sidebars.right.items.time_counter.y + 5);
}

void SinglePlayState::draw(GraphicsContext& gcx)
{
    gcx.drawTexture(tex_background, {0, 0, gcx.screenWidth(), gcx.screenHeight()});

    drawWell(gcx);
    drawLeftSidebar(gcx);
    drawRightSidebar(gcx);
}
