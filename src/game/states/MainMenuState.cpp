#include "MainMenuState.h"

#include "SinglePlayState.h"
#include "game/AppContext.h"
#include "game/components/PieceFactory.h"
#include "game/components/rotations/SRS.h"
#include "system/GraphicsContext.h"
#include "system/InputEvent.h"
#include "system/Localize.h"
#include "system/Texture.h"


// For regular mod() on negative numbers, the sign of the remainder is also negative (C++11).
// For ring vectors, we want to stay positive (unsigned).
int circularModulo(int num, int mod) {
    return (num % mod + mod) % mod;
}

MainMenuState::MainMenuState(AppContext& app)
    : tex_background(app.gcx().loadTexture("data/gamebg.png"))
    , tex_logo(app.gcx().loadTexture("data/logo.png"))
    , current_button_index(0)
{
    PieceFactory::changeInitialPositions(Rotations::SRS().initialPositions());

    buttons.emplace_back(app, tr("MARATHON"), [&app](){
        app.states().emplace(std::make_unique<SinglePlayState>(app));
    });
    buttons.emplace_back(app, tr("BATTLE"), [](){});
    buttons.emplace_back(app, tr("OPTIONS"), [](){});
    buttons.emplace_back(app, tr("EXIT"), [](){});

    buttons.at(current_button_index).onHoverEnter();
}

MainMenuState::~MainMenuState() = default;

void MainMenuState::update(const std::vector<InputEvent>& events, AppContext& app)
{
    const int center_y = app.gcx().screenHeight() / 2;
    const int left_x = app.gcx().screenWidth() * 0.1;

    buttons.at(0).setPosition(left_x + 20, center_y);
    for (unsigned i = 1; i < buttons.size(); i++) {
        const auto& prev = buttons.at(i - 1);
        buttons.at(i).setPosition(left_x + 20, prev.y() + prev.height() + 5);
    }

    for (const auto& event : events) {
        if (event.down()) {
            switch (event.type()) {
                case InputType::MENU_UP:
                    buttons.at(current_button_index).onHoverLeave();
                    current_button_index = circularModulo(static_cast<int>(current_button_index) - 1, buttons.size());
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
        }
    }
}

void MainMenuState::draw(GraphicsContext& gcx)
{
    tex_background->drawScaled({0, 0, gcx.screenWidth(), gcx.screenHeight()});

    const int center_y = gcx.screenHeight() / 2;
    const int left_x = gcx.screenWidth() * 0.1;

    tex_logo->drawScaled({left_x + 20, center_y - 260, 718, 180});
    for (const auto& btn : buttons)
        btn.draw(gcx);
}
