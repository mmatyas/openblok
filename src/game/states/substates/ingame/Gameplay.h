#pragma once

#include "game/ScoreTable.h"
#include "game/Transition.h"
#include "game/components/animations/BattleAttack.h"
#include "game/states/substates/Ingame.h"

#include <array>
#include <list>
#include <memory>
#include <stack>
#include <unordered_map>

class Font;
class Music;
class SoundEffect;
class TextPopup;
class Texture;


namespace SubStates {
namespace Ingame {
namespace States {

class Gameplay : public State {
public:
    Gameplay(AppContext&, IngameState&, unsigned short starting_gravity_level = 0);
    virtual ~Gameplay();

    void updateAnimationsOnly(IngameState&, AppContext&) final;
    void update(IngameState&, const std::vector<Event>&, AppContext&) final;
    void drawPassive(IngameState&, GraphicsContext&) const final;
    void drawActive(IngameState&, GraphicsContext&) const final;

private:
    const std::vector<DeviceID> player_devices;

    std::shared_ptr<Music> music;
    std::shared_ptr<Font> font_popuptext;
    std::shared_ptr<SoundEffect> sfx_onhold;
    std::shared_ptr<SoundEffect> sfx_onlevelup;
    std::array<std::shared_ptr<SoundEffect>, 4> sfx_onlineclear;
    std::shared_ptr<SoundEffect> sfx_onlock;
    std::shared_ptr<SoundEffect> sfx_onrotate;
    bool texts_need_update;

    std::shared_ptr<SoundEffect> sfx_ongameover;
    std::shared_ptr<SoundEffect> sfx_onfinish;

    const int lineclears_per_level;
    std::unordered_map<DeviceID, int> lineclears_left;
    std::unordered_map<DeviceID, std::stack<Duration>> gravity_levels;
    std::unordered_map<DeviceID, ScoreType> previous_lineclear_type;
    std::unordered_map<DeviceID, unsigned short> back2back_length;

    std::unordered_map<DeviceID, unsigned short> combo_length;
    std::unordered_map<DeviceID, bool> prev_piece_cleared_line;
    std::unordered_map<DeviceID, bool> current_piece_cleared_line;

    std::unordered_map<DeviceID, unsigned short> pending_garbage_lines;
    std::unordered_map<DeviceID, std::list<TextPopup>> textpopups;
    std::list<BattleAttackAnim> attackanims;

    Transition<unsigned> gameend_statistics_delay;

    enum class PlayerStatus : uint8_t {
        PLAYING,
        GAME_OVER,
        FINISHED,
    };
    std::unordered_map<DeviceID, PlayerStatus> player_status;

    std::vector<DeviceID> playingPlayers();
    void addNextPiece(IngameState&, DeviceID);
    void registerObservers(IngameState&, AppContext&);

    void increaseScoreMaybe(IngameState&, DeviceID, const WellEvent::lineclear_t&);
    void sendGarbageMaybe(IngameState&, DeviceID, const WellEvent::lineclear_t&);
    void increaseLevelMaybe(IngameState&, DeviceID, const WellEvent::lineclear_t&);
};

} // namespace States
} // namespace Ingame
} // namespace SubStates
