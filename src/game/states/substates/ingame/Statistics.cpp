#include "Statistics.h"

#include "FadeInOut.h"
#include "game/AppContext.h"
#include "game/layout/gameplay/PlayerArea.h"
#include "game/states/IngameState.h"
#include "game/util/DurationToString.h"
#include "system/Color.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Paths.h"


namespace SubStates {
namespace Ingame {
namespace States {

Statistics::Statistics(IngameState& parent, AppContext& app)
    : fadein_percent(
        std::chrono::seconds(1),
        [](double t){ return t; })
    , displayed_item_count(
        std::chrono::seconds(3),
        [this](double t){ return t * labels.size(); })
{
    const auto color = 0xEEEEEE_rgb;
    const auto color_highlight = 0xFEC500_rgb;
    auto font = app.gcx().loadFont(Paths::data() + "fonts/PTS55F.ttf", 26);
    auto font_highlight = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 26);
    auto font_title = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 30);

    tex_title = font_title->renderText(tr("STATISTICS"), color);

    labels.emplace_back(font->renderText(tr("Total Lines"), color));
    labels.emplace_back(font->renderText(tr("Singles"), color));
    labels.emplace_back(font->renderText(tr("Doubles"), color));
    labels.emplace_back(font->renderText(tr("Triples"), color));
    labels.emplace_back(font->renderText(tr("Perfects"), color));
    labels.emplace_back(font->renderText(tr("Back-to-Back"), color));
    labels.emplace_back(font->renderText(tr("Longest Back-to-Back"), color));
    labels.emplace_back(font->renderText(tr("T-Spin Minis"), color));
    labels.emplace_back(font->renderText(tr("T-Spin Mini Singles"), color));
    labels.emplace_back(font->renderText(tr("T-Spins"), color));
    labels.emplace_back(font->renderText(tr("T-Spin Singles"), color));
    labels.emplace_back(font->renderText(tr("T-Spin Doubles"), color));
    labels.emplace_back(font->renderText(tr("T-Spin Triples"), color));
    labels.emplace_back(font->renderText(tr("Duration"), color));
    labels.emplace_back(font_highlight->renderText(tr("Level"), color_highlight));
    labels.emplace_back(font_highlight->renderText(tr("Final Score"), color_highlight));

    for (const DeviceID device_id : parent.device_order) {
        auto& stats = parent.player_stats.at(device_id);
        auto& event_cnt = stats.event_count;
        auto& texs = scores[device_id];

        texs.emplace_back(font->renderText(std::to_string(stats.total_cleared_lines), color));
        texs.emplace_back(font->renderText(std::to_string(event_cnt[ScoreType::CLEAR_SINGLE]), color));
        texs.emplace_back(font->renderText(std::to_string(event_cnt[ScoreType::CLEAR_DOUBLE]), color));
        texs.emplace_back(font->renderText(std::to_string(event_cnt[ScoreType::CLEAR_TRIPLE]), color));
        texs.emplace_back(font->renderText(std::to_string(event_cnt[ScoreType::CLEAR_PERFECT]), color));

        texs.emplace_back(font->renderText(std::to_string(stats.back_to_back_count), color));
        texs.emplace_back(font->renderText(std::to_string(stats.back_to_back_longest), color));

        texs.emplace_back(font->renderText(std::to_string(event_cnt[ScoreType::MINI_TSPIN]), color));
        texs.emplace_back(font->renderText(std::to_string(event_cnt[ScoreType::CLEAR_MINI_TSPIN_SINGLE]), color));
        texs.emplace_back(font->renderText(std::to_string(event_cnt[ScoreType::TSPIN]), color));
        texs.emplace_back(font->renderText(std::to_string(event_cnt[ScoreType::CLEAR_TSPIN_SINGLE]), color));
        texs.emplace_back(font->renderText(std::to_string(event_cnt[ScoreType::CLEAR_TSPIN_DOUBLE]), color));
        texs.emplace_back(font->renderText(std::to_string(event_cnt[ScoreType::CLEAR_TSPIN_TRIPLE]), color));

        texs.emplace_back(font->renderText(Timing::toString(stats.gametime), color));
        texs.emplace_back(font_highlight->renderText(std::to_string(stats.level), color_highlight));
        texs.emplace_back(font_highlight->renderText(std::to_string(stats.score), color_highlight));
    }
}

void Statistics::update(IngameState& parent, const std::vector<Event>& events, AppContext& app)
{
    displayed_item_count.update(Timing::frame_duration);
    fadein_percent.update(Timing::frame_duration);
    if (fadein_percent.running())
        return;

    for (const auto& event : events) {
        if (event.type == EventType::INPUT && event.input.down()) {
            parent.states.emplace_back(std::make_unique<FadeOut>([&app](){
                app.states().pop();
            }));
            return;
        }
    }
}

void Statistics::drawBackground(IngameState& parent, GraphicsContext& gcx) const
{
    auto color = 0x2030FF00_rgba;
    color.a = fadein_percent.value() * 0xFF;

    for (const auto& ui_pa : parent.player_areas)
        gcx.drawFilledRect(ui_pa.second.wellBox(), color);
}

void Statistics::drawItems(IngameState& parent) const
{
    for (const DeviceID device_id : parent.device_order) {
        const auto& ui_pa = parent.player_areas.at(device_id);

        const int pos_x = ui_pa.wellBox().x;
        const int pos_x_right = ui_pa.wellBox().x + ui_pa.wellBox().w;
        int pos_y = ui_pa.wellBox().y;

        tex_title->drawAt(pos_x, pos_y);
        pos_y += tex_title->height() * 1.25;

        const auto& values = scores.at(device_id);
        assert(displayed_item_count.value() <= labels.size());
        assert(displayed_item_count.value() <= values.size());

        for (size_t i = 0; i < displayed_item_count.value(); i++) {
            const auto& label = labels.at(i);
            label->drawAt(pos_x, pos_y);

            const auto& value = values.at(i);
            value->drawAt(pos_x_right - value->width(), pos_y);

            pos_y += label->height();
        }
    }
}

void Statistics::drawPassive(IngameState& parent, GraphicsContext& gcx) const
{
    drawBackground(parent, gcx);
    drawItems(parent);
}

} // namespace States
} // namespace Ingame
} // namespace SubStates
