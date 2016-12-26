#include "Base.h"

#include "Options.h"
#include "game/AppContext.h"
#include "game/components/PieceFactory.h"
#include "game/components/rotations/SRS.h"
#include "game/states/MainMenuState.h"
#include "game/states/IngameState.h"
#include "game/util/CircularModulo.h"
#include "system/AudioContext.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Music.h"
#include "system/Paths.h"
#include "system/Texture.h"


namespace SubStates {
namespace MainMenu {

Base::Base(MainMenuState& parent, AppContext& app)
    : tex_background(app.gcx().loadTexture(Paths::data() + "gamebg.png"))
    , logo(app.gcx(), 150)
    , current_column(&primary_buttons)
    , column_slide_anim(std::chrono::milliseconds(350),
                        [](double t){ return t; },
                        [this](){  })
    , music(app.audio().loadMusic(Paths::data() + "music/menu.ogg"))
{
    PieceFactory::changeInitialPositions(Rotations::SRS().initialPositions());
    column_slide_anim.stop();

    primary_buttons.buttons.emplace_back(app, tr("SINGLEPLAYER"), [this](){
        openSubcolumn(&singleplayer_buttons);
    });
    {
        singleplayer_buttons.buttons.emplace_back(app, tr("MARATHON"), [this, &app](){
            startGame(app, GameMode::SP_MARATHON);
        });
        singleplayer_buttons.buttons.emplace_back(app, tr("SPRINT"), [this, &app](){
            startGame(app, GameMode::SP_40LINES);
        });
        singleplayer_buttons.buttons.emplace_back(app, tr("ULTRA"), [this, &app](){
            startGame(app, GameMode::SP_2MIN);
        });
        singleplayer_buttons.buttons.emplace_back(app, tr("MARATHON SIMPLE"), [this, &app](){
            startGame(app, GameMode::SP_MARATHON_SIMPLE);
        });
    }
    primary_buttons.buttons.emplace_back(app, tr("MULTIPLAYER"), [this](){
        openSubcolumn(&multiplayer_buttons);
    });
    {
        multiplayer_buttons.buttons.emplace_back(app, tr("BATTLE"), [this, &app](){
            startGame(app, GameMode::MP_BATTLE);
        });
        multiplayer_buttons.buttons.emplace_back(app, tr("MARATHON"), [this, &app](){
            startGame(app, GameMode::MP_MARATHON);
        });
        multiplayer_buttons.buttons.emplace_back(app, tr("MARATHON SIMPLE"), [this, &app](){
            startGame(app, GameMode::MP_MARATHON_SIMPLE);
        });
    }
    primary_buttons.buttons.emplace_back(app, tr("OPTIONS"), [&app, &parent](){
        parent.states.emplace_back(std::make_unique<SubStates::MainMenu::Options>(parent, app));
    });
    primary_buttons.buttons.emplace_back(app, tr("EXIT"), [&app](){
        app.states().pop();
    });

    primary_buttons.buttons.at(primary_buttons.selected_index).onHoverEnter();
    for (auto& btn : singleplayer_buttons.buttons)
        btn.setAlpha(0x0);
    for (auto& btn : multiplayer_buttons.buttons)
        btn.setAlpha(0x0);

    // move one of the rains lower
    auto& rain = rains.at(0);
    rain.setPosition(0, 48); // about 1.5 minos lower, TODO: Fix magic numbers

    music->playLoop();

    updatePositions(app.gcx());
}

Base::~Base() = default;

void Base::startGame(AppContext& app, GameMode gamemode)
{
    const auto duration = std::chrono::milliseconds(500);
    this->state_transition_alpha = std::make_unique<Transition<uint8_t>>(
        duration,
        [](double t){ return t * 0xFF; },
        [this, &app, gamemode](){
            this->onFadeoutComplete(app, std::make_unique<IngameState>(app, gamemode));
        }
    );
    music->fadeOut(duration);
}

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

    setColumnPosition(primary_buttons.buttons, left_x, center_y);
    setColumnPosition(singleplayer_buttons.buttons, left_x, center_y);
    setColumnPosition(multiplayer_buttons.buttons, left_x, center_y);
}

void Base::setColumnPosition(std::vector<Layout::MainMenuButton>& buttons, int left_x, int center_y)
{
    buttons.at(0).setPosition(left_x, center_y);
    for (unsigned i = 1; i < buttons.size(); i++) {
        const auto& prev = buttons.at(i - 1);
        buttons.at(i).setPosition(left_x, prev.y() + prev.height() + 5);
    }
}

void Base::openSubcolumn(ButtonColumn* subcolumn)
{
    assert(subcolumn != &primary_buttons);
    primary_buttons.buttons.at(primary_buttons.selected_index).onHoverLeave();
    current_column = subcolumn;
    current_column->buttons.at(current_column->selected_index).onHoverEnter();
    column_slide_anim.restart();
}

void Base::onFadeoutComplete(AppContext& app, std::unique_ptr<GameState>&& newstate)
{
    // reset the main menu to the primary button column
    current_column->buttons.at(current_column->selected_index).onHoverLeave();
    current_column = &primary_buttons;
    primary_buttons.buttons.at(primary_buttons.selected_index).onHoverEnter();
    updatePositions(app.gcx());
    for (auto& btn : primary_buttons.buttons)
        btn.setAlpha(0xFF);

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
    column_slide_anim.update(Timing::frame_duration);

    for (auto& rain : rains)
        rain.update();
}

Base::ButtonColumn::ButtonColumn()
    : selected_index(0)
{
}

void Base::ButtonColumn::selectNext()
{
    buttons.at(selected_index).onHoverLeave();
    selected_index++;
    selected_index %= buttons.size();
    buttons.at(selected_index).onHoverEnter();
}

void Base::ButtonColumn::selectPrev()
{
    buttons.at(selected_index).onHoverLeave();
    selected_index = circularModulo(static_cast<int>(selected_index) - 1, buttons.size());
    buttons.at(selected_index).onHoverEnter();
}

void Base::ButtonColumn::activate()
{
    buttons.at(selected_index).onPress();
}

void Base::update(MainMenuState& parent, const std::vector<Event>& events, AppContext& app)
{
    updateAnimationsOnly(parent, app);

    if (state_transition_alpha) {
        state_transition_alpha->update(Timing::frame_duration);
        return;
    }

    if (!column_slide_anim.running()) {
        for (const auto& event : events) {
            if (event.type == EventType::INPUT && event.input.down()) {
                switch (event.input.type()) {
                    case InputType::MENU_UP:
                        current_column->selectPrev();
                        break;
                    case InputType::MENU_DOWN:
                        current_column->selectNext();
                        break;
                    case InputType::MENU_OK:
                        current_column->activate();
                        break;
                    case InputType::MENU_CANCEL:
                        if (current_column != &primary_buttons) {
                            current_column->buttons.at(current_column->selected_index).onHoverLeave();
                            current_column = &primary_buttons;
                            primary_buttons.buttons.at(primary_buttons.selected_index).onHoverEnter();
                            column_slide_anim.restart();
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if (column_slide_anim.running()) {
        const int center_y = app.gcx().screenHeight() / 2;
        const int left_x = 20 + app.gcx().screenWidth() * 0.1;

        int slide_distance = -250; // main slides out left
        if (current_column == &primary_buttons)
            slide_distance = 250; // main slides in from left

        setColumnPosition(primary_buttons.buttons, left_x + slide_distance * column_slide_anim.value(), center_y);
        setColumnPosition(current_column->buttons, left_x - slide_distance * (1.f - column_slide_anim.value()), center_y);

        uint8_t alpha = column_slide_anim.value() * 0xFF;
        if (current_column != &primary_buttons)
            alpha = 0xFF - alpha;

        for (auto& btn : primary_buttons.buttons)
            btn.setAlpha(alpha);
        if (current_column != &primary_buttons) {
            for (auto& btn : current_column->buttons)
                btn.setAlpha(0xFF - alpha);
        }
    }
}

void Base::draw(MainMenuState&, GraphicsContext& gcx) const
{
    tex_background->drawScaled(screen_rect);

    for (const auto& rain : rains)
        rain.draw();

    logo.draw();

    for (const auto& btn : primary_buttons.buttons)
        btn.draw(gcx);
    if (current_column != &primary_buttons) {
        for (const auto& btn : current_column->buttons)
            btn.draw(gcx);
    }

    if (state_transition_alpha) {
        RGBAColor color = 0xFF_rgba;
        color.a = state_transition_alpha->value();
        gcx.drawFilledRect(screen_rect, color);
    }
}

} // namespace MainMenu
} // namespace SubStates
