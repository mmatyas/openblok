#include "Pause.h"

#include "Countdown.h"
#include "FadeInOut.h"
#include "Gameplay.h"
#include "game/AppContext.h"
#include "game/layout/gameplay/PlayerArea.h"
#include "game/states/IngameState.h"
#include "system/AudioContext.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Paths.h"


namespace SubStates {
namespace Ingame {
namespace States {

Pause::Pause(AppContext& app)
    : current_menuitem(0)
{
    app.audio().pauseAll();

    auto font_big = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 45);
    auto font_smaller = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 30);

    const auto color_normal = app.theme().colors.text;
    const auto color_highlight = 0xCE8000_rgb;

    tex_pause = font_big->renderText(tr("PAUSE"), color_normal);

    const std::vector<std::string> menuitems = {tr("RESUME"), tr("RESTART"), tr("QUIT")};
    for (const auto& label : menuitems) {
        tex_menuitems.emplace_back(std::array<std::unique_ptr<Texture>, 2>{
            font_smaller->renderText(label, color_normal),
            font_smaller->renderText(label, color_highlight)
        });
    }
}

void Pause::update(IngameState& parent, const std::vector<Event>& events, AppContext& app)
{
    for (const auto& event : events) {
        if (event.type == EventType::INPUT && event.input.down()) {
            switch (event.input.type()) {
                // exit pause mode
                case InputType::GAME_PAUSE:
                    assert(parent.states.size() > 1); // there should be a Gameplay state below
                    parent.states.pop_back();
                    return;
                case InputType::MENU_OK:
                    assert(current_menuitem < tex_menuitems.size());
                    switch (current_menuitem) { // TODO: fix magic numbers
                        case 0: // resume
                            assert(parent.states.size() > 1);
                            parent.states.pop_back();
                            return;
                        case 1: // restart
                            parent.states.emplace_back(std::make_unique<FadeOut>([&app, &parent](){
                                parent.states.erase(parent.states.begin(), --parent.states.end());
                                parent.states.emplace_back(std::make_unique<Gameplay>(app, parent));
                                parent.states.emplace_back(std::make_unique<Countdown>(app));
                                parent.states.emplace_back(std::make_unique<FadeIn>([&parent](){
                                    parent.states.pop_back();
                                }));
                            }));
                            return;
                        case 2: // back to main menu
                            parent.states.emplace_back(std::make_unique<FadeOut>([&app](){
                                app.states().pop();
                            }));
                            return;
                        default:
                            assert(false);
                    }
                    break;
                case InputType::MENU_UP:
                    if (current_menuitem > 0)
                        current_menuitem--;
                    break;
                case InputType::MENU_DOWN:
                    if (current_menuitem + 1 < tex_menuitems.size())
                        current_menuitem++;
                    break;
                default:
                    break;
            }
        }
    }
}

void Pause::drawActive(IngameState& parent, GraphicsContext&) const
{
    for (const auto& ui_pa : parent.player_areas) {
        const int center_x = ui_pa.second.wellCenterX();
        const int center_y = ui_pa.second.wellCenterY();
        tex_pause->drawAt(
            center_x - tex_pause->width() / 2,
            center_y - tex_pause->height() / 2);

        for (size_t i = 0; i < tex_menuitems.size(); i++) {
            const auto& tex = tex_menuitems.at(i).at(i == current_menuitem ? 1 : 0);
            tex->drawAt(
                center_x - tex->width() / 2,
                center_y + 150 + i * tex->height());
        }
    }
}

} // namespace States
} // namespace Ingame
} // namespace SubStates
