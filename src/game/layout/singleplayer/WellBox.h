#pragma once

#include "game/components/Well.h"
#include "game/layout/Box.h"

#include <array>
#include <vector>


class AppContext;
class Font;
class SoundEffect;
class Texture;

namespace SubStates { namespace SinglePlayer {
enum class StateType : uint8_t;
} }


namespace Layout {
class WellBox : public Layout::Box {
public:
    WellBox(AppContext&);

    void setPosition(int x, int y) override;
    void update(const std::vector<Event>&, SubStates::SinglePlayer::StateType current_state);
    void draw(GraphicsContext&, SubStates::SinglePlayer::StateType current_state) const;

    Well& well() { return m_well; }

private:
    Well m_well;

    bool gameover;
    Transition<double> gameover_background;

    std::shared_ptr<Font> font_big;
    std::unique_ptr<Texture> tex_pause;
    std::unique_ptr<Texture> tex_gameover;
    std::shared_ptr<SoundEffect> sfx_ongameover;

    Transition<uint8_t> countdown;
    std::array<std::unique_ptr<Texture>, 3> tex_countdown;

    static constexpr uint8_t border_width = 5;
    ::Rectangle border_left;
    ::Rectangle border_right;
    ::Rectangle border_top;
    ::Rectangle border_bottom;
};
} // namespace Layout
