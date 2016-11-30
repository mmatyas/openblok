#include "Options.h"

#include "game/AppContext.h"
#include "game/GameConfigFile.h"
#include "game/layout/options/OptionsItem.h"
#include "game/layout/options/ToggleBtn.h"
#include "game/layout/options/ValueChooser.h"
#include "game/states/MainMenuState.h"
#include "game/util/CircularModulo.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Paths.h"
#include "system/Texture.h"

#include <assert.h>
#include <algorithm>


namespace SubStates {
namespace MainMenu {

Options::Options(MainMenuState& parent, AppContext& app)
    : current_category_idx(0)
    , current_setting_idx(0)
    , current_subitem(nullptr)
{
    category_buttons.emplace_back(app, tr("GENERAL"));
    category_buttons.emplace_back(app, tr("FINE TUNING"));
    // category_buttons.emplace_back(app, tr("INPUT"));
    category_buttons.at(current_category_idx).onHoverEnter();

    using ToggleButton = Layout::Options::ToggleButton;
    using ValueChooser = Layout::Options::ValueChooser;

    /* Note: the following structures aren't too complex,
     * it's just that they need a lot of ctor parameters,
     * like long strings, arrays and lambda callbacks */

    std::vector<std::unique_ptr<Layout::Options::OptionsItem>> system_options;
    {
        system_options.emplace_back(std::make_unique<ToggleButton>(
            app, app.sysconfig().fullscreen, tr("Fullscreen mode"),
            tr("Toggle fullscreen mode. On certain (embedded) devices, this setting may have no effect."),
            [&app](bool val){
                app.window().toggleFullscreen();
                app.sysconfig().fullscreen = val;
            }));
        system_options.back()->setMarginBottom(40);
        system_options.emplace_back(std::make_unique<ToggleButton>(
            app, app.sysconfig().sfx, tr("Sound effects"),
            tr("Enable or disable sound effects."),
            [&app](bool val){
                app.sysconfig().sfx = val;
                // TODO
            }));
        system_options.emplace_back(std::make_unique<ToggleButton>(
            app, app.sysconfig().music, tr("Background music"),
            tr("Enable or disable the background music."),
            [&app](bool val){
                app.sysconfig().music = val;
                // TODO
            }));
    }
    subitem_panels.push_back(std::move(system_options));

    std::vector<std::unique_ptr<Layout::Options::OptionsItem>> tuning_options;
    {
        tuning_options.emplace_back(std::make_unique<ValueChooser>(app,
            std::vector<std::string>({tr("SRS"), tr("TGM"), tr("Classic")}),
            app.wellconfig().rotation_style == RotationStyle::SRS ?
                0 : (app.wellconfig().rotation_style == RotationStyle::TGM ? 1 : 2),
            tr("Rotation style"),
            std::string(tr("SRS: The rotation style used by most commercial falling block games.\n")) +
                tr("TGM: A popular style common in far eastern games and arcade machines.\n") +
                tr("Classic: The rotation style of old console games; it does not allow wall kicking."),
            [&app](const std::string& val){
                static const std::unordered_map<std::string, RotationStyle> map = {
                    {tr("SRS"), RotationStyle::SRS},
                    {tr("TGM"), RotationStyle::TGM},
                    {tr("Classic"), RotationStyle::CLASSIC},
                };
                app.wellconfig().rotation_style = map.at(val);
            }));

        std::vector<std::string> das_values(20);
        int k = 0;
        std::generate(das_values.begin(), das_values.end(), [&k]{ return std::to_string(++k) + "/60 s"; });
        auto das_repeat_values = das_values;
        tuning_options.emplace_back(std::make_unique<ValueChooser>(app, std::move(das_values),
            app.wellconfig().shift_normal - 1, // num to offset
            tr("DAS initial delay"),
            tr("The time it takes to turn on horizontal movement autorepeat."),
            [&app](const std::string& val){
                // this must not throw error
                app.wellconfig().shift_normal = std::stoul(val.substr(0, val.find("/") - 1));
            }));
        tuning_options.emplace_back(std::make_unique<ValueChooser>(app, std::move(das_repeat_values),
            app.wellconfig().shift_turbo - 1, // num to offset
            tr("DAS repeat delay"),
            tr("Horizontal movement delay during autorepeat."),
            [&app](const std::string& val){
                // this must not throw error
                app.wellconfig().shift_turbo = std::stoul(val.substr(0, val.find("/") - 1));
            }));

        tuning_options.emplace_back(std::make_unique<ToggleButton>(app,
            !app.wellconfig().instant_harddrop, // sonic drop == not instant hard drop
            tr("Sonic drop"),
            tr("If set to 'ON', hard drop does not lock the piece instantly."),
            [&app](bool val){ app.wellconfig().instant_harddrop = !val; }));
        tuning_options.emplace_back(std::make_unique<ValueChooser>(app,
            std::vector<std::string>({tr("Instant"), tr("Extended"), tr("Infinite")}),
            app.wellconfig().lock_delay_type == LockDelayType::CLASSIC ?
                0 : (app.wellconfig().lock_delay_type == LockDelayType::EXTENDED ? 1 : 2),
            tr("Piece lock style"),
            std::string(tr("Instant: The piece locks instantly when it reaches the ground.\n")) +
                tr("Extended: You can move or rotate the piece 10 times before it locks.\n") +
                tr("Infinite: You can move or rotate the piece an infinite number of times."),
            [&app](const std::string& val){
                static const std::unordered_map<std::string, LockDelayType> map = {
                    {tr("Instant"), LockDelayType::CLASSIC},
                    {tr("Extended"), LockDelayType::EXTENDED},
                    {tr("Infinite"), LockDelayType::INFINITE},
                };
                app.wellconfig().lock_delay_type = map.at(val);
            }));

        k = 0;
        std::vector<std::string> lockdelay_values(60);
        std::generate(lockdelay_values.begin(), lockdelay_values.end(), [&k]{ return std::to_string(++k) + "/60 s"; });
        tuning_options.emplace_back(std::make_unique<ValueChooser>(app,
            std::move(lockdelay_values),
            app.wellconfig().lock_delay - 1, // num to offset
            tr("Lock delay"),
            tr("The time while you can still move the piece after it reaches the ground. See 'Piece lock style'."),
            [&app](const std::string& val){
                // this must not throw error
                app.wellconfig().shift_normal = std::stoul(val.substr(0, val.find("/") - 1));
            }));

        tuning_options.emplace_back(std::make_unique<ToggleButton>(app,
            app.wellconfig().tspin_enabled,
            tr("Enable T-Spins"),
            tr("Allow T-Spin detection and scoring. Works best with SRS rotation."),
            [&app](bool val){ app.wellconfig().tspin_enabled = val; }));
        tuning_options.emplace_back(std::make_unique<ToggleButton>(app,
            app.wellconfig().tspin_allow_wallblock,
            tr("Detect T-Spins at the walls"),
            tr("Allow detecting T-Spins even when a corner of the T-Slot is outside of the Well.\nRequires the 'Enable T-Spins' option."),
            [&app](bool val){ app.wellconfig().tspin_allow_wallblock = val; }));
        tuning_options.emplace_back(std::make_unique<ToggleButton>(app,
            app.wellconfig().tspin_allow_wallkick,
            tr("Allow T-Spins by wallkick"),
            tr("Allow detecting T-Spins created by wall kicking.\nRequires the 'Enable T-Spins' option."),
            [&app](bool val){ app.wellconfig().tspin_allow_wallkick = val; }));
    }
    subitem_panels.push_back(std::move(tuning_options));


    updatePositions(app.gcx());

    fn_category_input = [this, &parent, &app](InputType input){
        assert(!current_subitem);
        switch (input) {
            case InputType::MENU_OK:
                current_input_handler = &fn_settings_input;
                current_subitem = subitem_panels.at(current_category_idx).front().get();
                current_subitem->onHoverEnter();
                break;
            case InputType::MENU_CANCEL:
                GameConfigFile::save(app.sysconfig(), app.wellconfig(), Paths::config() + "game.cfg");
                assert(parent.states.size() > 1);
                parent.states.pop_back();
                break;
            case InputType::MENU_UP:
                category_buttons.at(current_category_idx).onHoverLeave();
                current_category_idx = circularModulo(
                    static_cast<int>(current_category_idx) - 1, category_buttons.size());
                category_buttons.at(current_category_idx).onHoverEnter();
                break;
            case InputType::MENU_DOWN:
                category_buttons.at(current_category_idx).onHoverLeave();
                current_category_idx = circularModulo(
                    static_cast<int>(current_category_idx) + 1, category_buttons.size());
                category_buttons.at(current_category_idx).onHoverEnter();
                break;
            default:
                break;
        }
    };
    fn_settings_input = [this](InputType input){
        assert(current_subitem);
        auto& panel = subitem_panels.at(current_category_idx);
        switch (input) {
            case InputType::MENU_CANCEL:
                current_subitem->onHoverLeave();
                current_subitem = nullptr;
                current_setting_idx = 0;
                current_input_handler = &fn_category_input;
                break;
            case InputType::MENU_UP:
                current_subitem->onHoverLeave();
                current_setting_idx = circularModulo(
                    static_cast<int>(current_setting_idx) - 1, panel.size());
                current_subitem = panel.at(current_setting_idx).get();
                current_subitem->onHoverEnter();
                break;
            case InputType::MENU_DOWN:
                current_subitem->onHoverLeave();
                current_setting_idx = circularModulo(
                    static_cast<int>(current_setting_idx) + 1, panel.size());
                current_subitem = panel.at(current_setting_idx).get();
                current_subitem->onHoverEnter();
                break;
            case InputType::MENU_LEFT:
                current_subitem->onLeftPress();
                break;
            case InputType::MENU_RIGHT:
                current_subitem->onRightPress();
                break;
            default:
                break;
        }
    };
    current_input_handler = &fn_category_input;
}

Options::~Options() = default;

void Options::updatePositions(GraphicsContext& gcx)
{
    const int width = gcx.screenWidth() * 0.90;
    const int height = 600; // TODO: fix magic numbers
    container_rect = {
        (gcx.screenWidth() - width) / 2, (gcx.screenHeight() - height) / 2,
        width, height
    };
    screen_rect = {0, 0, gcx.screenWidth(), gcx.screenHeight()};

    const int inner_x = container_rect.x + 12;
    category_buttons.at(0).setPosition(inner_x, container_rect.y + 12);
    for (unsigned i = 1; i < category_buttons.size(); i++) {
        const auto& prev = category_buttons.at(i - 1);
        category_buttons.at(i).setPosition(inner_x, prev.y() + prev.height() + 6);
    }

    const int category_column_width = category_buttons.at(0).width() + 12;
    const int subpanel_x = container_rect.x + category_column_width + 30;
    const int subpanel_item_width = container_rect.w - category_column_width - 30 * 2;
    for (auto& subpanel : subitem_panels) {
        subpanel.at(0)->setPosition(subpanel_x, container_rect.y + 30);
        subpanel.at(0)->setWidth(subpanel_item_width);
        for (unsigned i = 1; i < subpanel.size(); i++) {
            const auto& prev = subpanel.at(i - 1);
            subpanel.at(i)->setPosition(subpanel_x, prev->y() + prev->height() + prev->marginBottom());
            subpanel.at(i)->setWidth(subpanel_item_width);
        }
    }
}

void Options::update(MainMenuState& parent, const std::vector<Event>& events, AppContext& app)
{
    parent.states.front()->updateAnimationsOnly(parent, app);

    for (const auto& event : events) {
        switch (event.type) {
            case EventType::INPUT:
                if (!event.input.down())
                    continue;
                (*current_input_handler)(event.input.type());
                break;
            default:
                break;
        }
    }
}

void Options::draw(MainMenuState& parent, GraphicsContext& gcx) const
{
    parent.states.front()->draw(parent, gcx);

    RGBAColor black = 0x00000080_rgba;
    gcx.drawFilledRect(screen_rect, black);
    RGBAColor panel_bg = 0x002060F0_rgba;
    gcx.drawFilledRect(container_rect, panel_bg);

    for (const auto& btn : category_buttons)
        btn.draw(gcx);

    assert(current_category_idx < subitem_panels.size());
    for (const auto& btn : subitem_panels.at(current_category_idx))
        btn->draw(gcx);

    if (current_subitem) {
        auto& description_tex = current_subitem->descriptionTex();
        auto bgrect = container_rect;
        bgrect.h = description_tex->height() + 10;
        bgrect.y += container_rect.h - bgrect.h;
        gcx.drawFilledRect(bgrect, black);
        description_tex->drawAt(bgrect.x + 20, bgrect.y + 5);
    }
}

} // namespace MainMenu
} // namespace SubStates
