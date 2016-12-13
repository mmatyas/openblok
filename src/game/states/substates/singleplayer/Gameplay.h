#pragma once

#include "game/ScoreTable.h"
#include "game/Transition.h"
#include "game/states/substates/SinglePlayer.h"

#include <array>
#include <memory>
#include <stack>

class Font;
class Music;
class SoundEffect;
class TextPopup;
class Texture;


namespace SubStates {
namespace SinglePlayer {
namespace States {

class Gameplay : public State {
public:
    Gameplay(SinglePlayState&, AppContext&);
    ~Gameplay();
    void updateAnimationsOnly(SinglePlayState&, AppContext&) final;
    void update(SinglePlayState&, const std::vector<Event>&, AppContext&) final;
    void draw(SinglePlayState&, GraphicsContext&) const final;

private:
    std::shared_ptr<Music> music;
    std::shared_ptr<SoundEffect> sfx_onhold;
    std::shared_ptr<SoundEffect> sfx_onlevelup;
    std::array<std::shared_ptr<SoundEffect>, 4> sfx_onlineclear;
    std::shared_ptr<SoundEffect> sfx_onlock;
    std::shared_ptr<SoundEffect> sfx_onrotate;
    std::shared_ptr<Font> font_popuptext;
    bool texts_need_update;

    Transition<void> pending_levelup_msg;
    std::vector<std::unique_ptr<TextPopup>> textpopups;

    const unsigned lineclears_per_level;
    int lineclears_left;
    std::stack<Duration> gravity_levels;
    ScoreType previous_lineclear_type;
    unsigned short back2back_length;

    void addNextPiece(SinglePlayState& parent);
    void registerObservers(SinglePlayState& parent, AppContext&);
};

} // namespace States
} // namespace SinglePlayer
} // namespace SubStates
