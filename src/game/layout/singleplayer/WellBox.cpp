#include "WellBox.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "system/Font.h"
#include "system/Localize.h"


namespace Layout {

WellBox::WellBox(AppContext& app)
    : gameover(false)
    , gameover_background(std::chrono::seconds(2), [](double t){ return t; })
{
    m_well.registerObserver(WellEvent::Type::GAME_OVER, [this](const WellEvent&){
        gameover = true;
        gameover_background.restart();
    });

    m_well.setRotationFn(app.scripts().loadRotationFn("data/rotations/srs.lua"));

    bounding_box.w = 10 * Mino::texture_size_px + border_width * 2;
    bounding_box.h = 20.3 * Mino::texture_size_px + border_width * 2;

    font_big = app.gcx().loadFont("data/fonts/PTC75F.ttf", 45);
    tex_pause = font_big->renderText(tr("PAUSE"), 0xEEEEEE_rgb);
    tex_gameover = font_big->renderText(tr("GAME OVER"), 0xEEEEEE00_rgba);

    setPosition(0, 0);
    gameover_background.stop();
}

void WellBox::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;

    border_left = { x, y, border_width, height() };
    border_right = { x + width() - border_width, y, border_width, height() };
    border_top = { x + border_width, y, width() - border_width * 2, border_width };
    border_bottom = { x + border_width, y + height() - border_width,
                      width() - border_width * 2, border_width };
}


void WellBox::update(const std::vector<InputEvent>& events, AppContext& app)
{
    m_well.update(events, app);
    if (gameover) {
        gameover_background.update(Timing::frame_duration);
        if (gameover_background.running()) {
            RGBAColor color = 0xEEEEEE00_rgba;
            if (gameover_background.value() > 0.4)
                color.a = std::min<int>(0xFF, (gameover_background.value() - 0.4) * 0x1FF);
            tex_gameover = font_big->renderText(tr("GAME OVER"), color);
        }
    }
}

void WellBox::draw(GraphicsContext& gcx, bool paused) const
{
    m_well.drawBackground(gcx, x() + border_width, y() + border_width);
    if (gameover) {
        m_well.drawContent(gcx, x() + border_width, y() + border_width);

        int box_h = (height() - border_width * 2) * gameover_background.value();
        gcx.drawFilledRect({
            x() + border_width, y() + height() - border_width - box_h,
            width() - border_width * 2, box_h
        }, 0xA0_rgba);

        tex_gameover->drawAt(x() + (width() - static_cast<int>(tex_gameover->width())) / 2,
                            y() + (height() - static_cast<int>(tex_gameover->height())) / 2);
    }
    else if (paused)
        tex_pause->drawAt(x() + (width() - static_cast<int>(tex_pause->width())) / 2,
                          y() + (height() - static_cast<int>(tex_pause->height())) / 2);
    else {
        m_well.drawContent(gcx, x() + border_width, y() + border_width);
    }

    static const auto boardborder_color = 0x1A3A8A_rgb;
    gcx.drawFilledRect(border_left, boardborder_color);
    gcx.drawFilledRect(border_right, boardborder_color);
    gcx.drawFilledRect(border_top, boardborder_color);
    gcx.drawFilledRect(border_bottom, boardborder_color);
}

} // namespace Layout
