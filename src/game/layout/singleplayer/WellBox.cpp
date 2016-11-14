#include "WellBox.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "game/components/rotations/SRS.h"
#include "game/states/substates/SinglePlayer.h"
#include "system/AudioContext.h"
#include "system/Font.h"
#include "system/Localize.h"
#include "system/Paths.h"
#include "system/SoundEffect.h"


namespace Layout {

WellBox::WellBox(AppContext& app)
    : gameover(false)
    , gameover_background(std::chrono::seconds(2),
        [](double t){ return t; },
        [this](){ sfx_ongameover->playOnce(); })
    , sfx_ongameover(app.audio().loadSound(Paths::data() + "sfx/gameover.ogg"))
    , countdown(std::chrono::milliseconds(2400),
                [](double t){ return t * 2.9999; })
{
    m_well.registerObserver(WellEvent::Type::GAME_OVER, [this](const WellEvent&){
        gameover = true;
        gameover_background.restart();
    });

    m_well.setRotationFn(std::make_unique<Rotations::SRS>()); // TODO: make a shared rotation store

    bounding_box.w = 10 * Mino::texture_size_px + border_width * 2;
    bounding_box.h = 20.3 * Mino::texture_size_px + border_width * 2;

    font_big = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 45);
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


void WellBox::update(const std::vector<Event>& events, SubStates::SinglePlayer::StateType current_state)
{
    // filter the input events only
    std::vector<InputEvent> input_events;
    for (const auto& event : events) {
        if (event.type == EventType::INPUT)
            input_events.emplace_back(event.input.type(), event.input.down());
    }

    m_well.updateKeystateOnly(input_events);

    using StateType = SubStates::SinglePlayer::StateType;
    switch (current_state) {
        case StateType::GAME_RUNNING:
            m_well.updateGameplayOnly(input_events);
            if (gameover) {
                gameover_background.update(Timing::frame_duration);
                if (gameover_background.running()) {
                    RGBAColor color = 0xEEEEEE00_rgba;
                    if (gameover_background.value() > 0.4)
                        color.a = std::min<int>(0xFF, (gameover_background.value() - 0.4) * 0x1FF);
                    tex_gameover = font_big->renderText(tr("GAME OVER"), color);
                }
            }
            break;
        default:
            break;
    }
}

void WellBox::draw(GraphicsContext& gcx, SubStates::SinglePlayer::StateType current_state) const
{
    m_well.drawBackground(gcx, x() + border_width, y() + border_width);

    using StateType = SubStates::SinglePlayer::StateType;
    switch (current_state) {
        case StateType::GAME_RUNNING:
            m_well.drawContent(gcx, x() + border_width, y() + border_width);
            if (gameover) {
                int box_h = (height() - border_width * 2) * gameover_background.value();
                gcx.drawFilledRect({
                    x() + border_width, y() + height() - border_width - box_h,
                    width() - border_width * 2, box_h
                }, 0xA0_rgba);

                tex_gameover->drawAt(x() + (width() - static_cast<int>(tex_gameover->width())) / 2,
                                     y() + (height() - static_cast<int>(tex_gameover->height())) / 2);
            }
        case StateType::FINISHED:
            // TODO
            break;
        default:
            break;
    }

    static const auto boardborder_color = 0x1A3A8A_rgb;
    gcx.drawFilledRect(border_left, boardborder_color);
    gcx.drawFilledRect(border_right, boardborder_color);
    gcx.drawFilledRect(border_top, boardborder_color);
    gcx.drawFilledRect(border_bottom, boardborder_color);
}

} // namespace Layout
