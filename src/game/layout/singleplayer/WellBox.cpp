#include "WellBox.h"

#include "game/AppContext.h"
#include "game/components/rotations/SRS.h"
#include "game/states/substates/SinglePlayer.h"
#include "system/AudioContext.h"
#include "system/Font.h"
#include "system/Localize.h"
#include "system/Paths.h"
#include "system/SoundEffect.h"


namespace Layout {

WellBox::WellBox(AppContext&)
{
    m_well.setRotationFn(std::make_unique<Rotations::SRS>()); // TODO: make a shared rotation store

    bounding_box.w = wellWidth() + border_width * 2;
    bounding_box.h = wellHeight() + border_width * 2;

    setPosition(0, 0);
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
        case StateType::GAME_OVER:
        case StateType::GAME_RUNNING:
            m_well.drawContent(gcx, x() + border_width, y() + border_width);
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
