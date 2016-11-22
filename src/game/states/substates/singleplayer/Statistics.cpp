#include "Statistics.h"

#include "game/AppContext.h"
#include "game/ScoreTable.h"
#include "game/states/SinglePlayState.h"
#include "game/util/DurationToString.h"
#include "system/Color.h"
#include "system/Font.h"
#include "system/Localize.h"
#include "system/Paths.h"
#include "system/Texture.h"


namespace SubStates {
namespace SinglePlayer {
namespace States {

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
        if (event.type == EventType::INPUT && event.input.down()) {
            if (displayed_item_count.value() != score_texs.size()) {
                background_percent.update(background_percent.length());
                title_alpha.update(title_alpha.length());
                displayed_item_count.update(displayed_item_count.length());
                return; // stop parsing the rest of the keys
            }
            state_transition_alpha = std::make_unique<Transition<uint8_t>>(
                std::chrono::milliseconds(500),
                [](double t){ return t * 0xFF; },
                [&app](){ app.states().pop(); }
            );
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

} // namespace States
} // namespace SinglePlayer
} // namespace SubStates
