#include "Base.h"

// #include "OptionsState.h"
#include "game/AppContext.h"
#include "game/components/PieceFactory.h"
#include "game/components/rotations/SRS.h"
#include "game/states/SinglePlayState.h"
#include "game/util/CircularModulo.h"
#include "system/AudioContext.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Music.h"
#include "system/Paths.h"
#include "system/Texture.h"


namespace SubStates {
namespace MainMenu {

Base::Base(MainMenuState&, AppContext& app)
    : tex_background(app.gcx().loadTexture(Paths::data() + "gamebg.png"))
    , logo(app.gcx(), 150)
    , current_button_index(0)
    , music(app.audio().loadMusic(Paths::data() + "music/menu.ogg"))
{
    PieceFactory::changeInitialPositions(Rotations::SRS().initialPositions());

    buttons.emplace_back(app, tr("MARATHON"), [this, &app](){
        const auto duration = std::chrono::milliseconds(500);
        this->state_transition_alpha = std::make_unique<Transition<uint8_t>>(
            duration,
            [](double t){ return t * 0xFF; },
            [this, &app](){ this->onFadeoutComplete(app, std::make_unique<SinglePlayState>(app)); }
        );
        music->fadeOut(duration);
    });
    buttons.emplace_back(app, tr("BATTLE"), [](){});
    buttons.emplace_back(app, tr("OPTIONS"), [&app](){
        //app.states().emplace(std::make_unique<OptionsState>(app));
    });
    buttons.emplace_back(app, tr("EXIT"), [&app](){
        app.states().pop();
    });

    buttons.at(current_button_index).onHoverEnter();

    // move one of the rains lower
    auto& rain = rains.at(0);
    rain.setPosition(0, 48); // about 1.5 minos lower, TODO: Fix magic numbers

    music->playLoop();

    updatePositions(app.gcx());
}

Base::~Base() = default;

void Base::updatePositions(GraphicsContext& gcx)
{
    screen_rect = {0, 0, gcx.screenWidth(), gcx.screenHeight()};

    const int center_y = gcx.screenHeight() / 2;
    const int left_x = 20 + gcx.screenWidth() * 0.1;
    const int right_x = gcx.screenWidth() - left_x + 32; // move 1 mino left, TODO: Fix magic numbers

    logo.setPosition(left_x, center_y - 260);

    int rain_x = right_x;
    for (auto& rain : rains) {
        rain_x -= rain.width() + 10;
        rain.setPosition(rain_x, rain.y());
        rain.setHeight(gcx.screenHeight());
    }

    buttons.at(0).setPosition(left_x, center_y);
    for (unsigned i = 1; i < buttons.size(); i++) {
        const auto& prev = buttons.at(i - 1);
        buttons.at(i).setPosition(left_x, prev.y() + prev.height() + 5);
    }
}

void Base::onFadeoutComplete(AppContext& app, std::unique_ptr<GameState>&& newstate)
{
    // this transition will run when we return to the menu
    state_transition_alpha = std::make_unique<Transition<uint8_t>>(
        std::chrono::milliseconds(500),
        [](double t){ return (1.0 - t) * 0xFF; },
        [this](){
            music->playLoop();
            this->state_transition_alpha.reset();
        });
    app.states().emplace(std::move(newstate));
}

void Base::updateAnimationsOnly(MainMenuState&, AppContext&)
{
    for (auto& rain : rains)
        rain.update();
}

void Base::update(MainMenuState& parent, const std::vector<Event>& events, AppContext& app)
{
    updateAnimationsOnly(parent, app);

    if (state_transition_alpha) {
        state_transition_alpha->update(Timing::frame_duration);
        return;
    }

    for (const auto& event : events) {
        switch (event.type) {
            case EventType::INPUT:
                if (!event.input.down())
                    continue;
                switch (event.input.type()) {
                    case InputType::MENU_UP:
                        buttons.at(current_button_index).onHoverLeave();
                        current_button_index = circularModulo(static_cast<int>(current_button_index) - 1,
                                                              buttons.size());
                        buttons.at(current_button_index).onHoverEnter();
                        break;
                    case InputType::MENU_DOWN:
                        buttons.at(current_button_index).onHoverLeave();
                        current_button_index++;
                        current_button_index %= buttons.size();
                        buttons.at(current_button_index).onHoverEnter();
                        break;
                    case InputType::MENU_OK:
                        buttons.at(current_button_index).onPress();
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

void Base::draw(MainMenuState&, GraphicsContext& gcx) const
{
    tex_background->drawScaled(screen_rect);

    for (const auto& rain : rains)
        rain.draw();

    logo.draw();
    for (const auto& btn : buttons)
        btn.draw(gcx);

    if (state_transition_alpha) {
        RGBAColor color = 0xFF_rgba;
        color.a = state_transition_alpha->value();
        gcx.drawFilledRect(screen_rect, color);
    }
}

} // namespace MainMenu
} // namespace SubStates
