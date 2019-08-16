#include "Options.h"

#include "game/AppContext.h"
#include "game/GameConfigFile.h"
#include "game/layout/options/DeviceChooser.h"
#include "game/layout/options/InputField.h"
#include "game/layout/options/OptionsItem.h"
#include "game/layout/options/ToggleBtn.h"
#include "game/layout/options/ValueChooser.h"
#include "game/states/MainMenuState.h"
#include "game/util/CircularModulo.h"
#include "system/AudioContext.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Paths.h"
#include "system/Texture.h"
#include "system/util/MakeUnique.h"

#include <tinydir_cxx.h>
#include <algorithm>
#include <unordered_map>
#include <assert.h>


namespace SubStates {
namespace MainMenu {

Options::Options(MainMenuState& parent, AppContext& app)
    : current_category_idx(0)
    , current_setting_idx(0)
    , current_subitem(nullptr)
    , current_device_id(-1) // keyboard
    , current_input_handler(nullptr)
{
    category_buttons.emplace_back(app, tr("GENERAL"));
    category_buttons.emplace_back(app, tr("FINE TUNING"));
    category_buttons.emplace_back(app, tr("INPUT (MENU)"));
    category_buttons.emplace_back(app, tr("INPUT (GAME)"));
    category_buttons.at(current_category_idx).onHoverEnter();

    using DeviceChooser = Layout::Options::DeviceChooser;
    using ToggleButton = Layout::Options::ToggleButton;
    using ValueChooser = Layout::Options::ValueChooser;

    /* Note: the following structures aren't too complex,
     * it's just that they need a lot of ctor parameters,
     * like long strings, arrays and lambda callbacks */

    std::vector<std::shared_ptr<Layout::Options::OptionsItem>> system_options;
    {
        system_options.emplace_back(std::make_shared<ToggleButton>(
            app, app.sysconfig().fullscreen, tr("Fullscreen mode"),
            tr("Toggle fullscreen mode. On certain (embedded) devices, this setting may have no effect."),
            [&app](bool val){
                app.window().toggleFullscreen();
                app.sysconfig().fullscreen = val;
            }));
        system_options.back()->setMarginBottom(40);
        system_options.emplace_back(std::make_shared<ToggleButton>(
            app, app.sysconfig().sfx, tr("Sound effects"),
            tr("Enable or disable sound effects."),
            [&app](bool val){
                app.sysconfig().sfx = val;
                app.audio().toggleSFXMute();
            }));
        system_options.emplace_back(std::make_shared<ToggleButton>(
            app, app.sysconfig().music, tr("Background music"),
            tr("Enable or disable the background music."),
            [&app](bool val){
                app.sysconfig().music = val;
                app.audio().toggleMusicMute();
            }));
        system_options.back()->setMarginBottom(40);

        auto detected_themes = detectedThemes();
        const size_t current_theme_idx = std::distance(detected_themes.begin(),
            std::find(detected_themes.begin(), detected_themes.end(), app.sysconfig().theme_dir_name));
        if (current_theme_idx >= detected_themes.size()) // if the dir was removed
            detected_themes.emplace_back(app.sysconfig().theme_dir_name);
        system_options.emplace_back(std::make_shared<ValueChooser>(app,
            std::move(detected_themes), current_theme_idx,
            tr("Theme"), tr("Change the graphical theme of the game."),
            [&app, &parent](const std::string& val){
                app.sysconfig().theme_dir_name = val;
                parent.reloadTheme(app);
            }));
    }
    subitem_panels.push_back(std::move(system_options));

    std::vector<std::shared_ptr<Layout::Options::OptionsItem>> tuning_options;
    {
        tuning_options.emplace_back(std::make_shared<ValueChooser>(app,
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
        tuning_options.emplace_back(std::make_shared<ValueChooser>(app, std::move(das_values),
            std::min<size_t>(das_values.size(), app.wellconfig().shift_normal) - 1, // num to offset
            tr("DAS initial delay"),
            tr("The time it takes to turn on horizontal movement autorepeat."),
            [&app](const std::string& val){
                // this must not throw error
                app.wellconfig().shift_normal = std::stoul(val.substr(0, val.find("/")));
            }));
        tuning_options.emplace_back(std::make_shared<ValueChooser>(app, std::move(das_repeat_values),
            std::min<size_t>(das_values.size(), app.wellconfig().shift_turbo) - 1, // num to offset
            tr("DAS repeat delay"),
            tr("Horizontal movement delay during autorepeat."),
            [&app](const std::string& val){
                // this must not throw error
                app.wellconfig().shift_turbo = std::stoul(val.substr(0, val.find("/")));
            }));

        tuning_options.emplace_back(std::make_shared<ToggleButton>(app,
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

        tuning_options.emplace_back(std::make_shared<ValueChooser>(app,
            std::vector<std::string>({"0","1","2","3","4","5"}),
            std::min<size_t>(das_values.size() - 1, app.wellconfig().max_next_pieces),
            tr("Next pieces"),
            tr("The number of next pieces you can preview."),
            [&app](const std::string& val){
                // this must not throw error
                app.wellconfig().max_next_pieces = std::stoul(val);
            }));

        k = 0;
        std::vector<std::string> lockdelay_values(60);
        std::generate(lockdelay_values.begin(), lockdelay_values.end(), [&k]{ return std::to_string(++k) + "/60 s"; });
        tuning_options.emplace_back(std::make_shared<ValueChooser>(app,
            std::move(lockdelay_values),
            std::min<size_t>(lockdelay_values.size(), app.wellconfig().lock_delay) - 1, // num to offset
            tr("Lock delay"),
            tr("The time while you can still move the piece after it reaches the ground. See 'Piece lock style'."),
            [&app](const std::string& val){
                // this must not throw error
                app.wellconfig().lock_delay = std::stoul(val.substr(0, val.find("/")));
            }));

        tuning_options.emplace_back(std::make_shared<ToggleButton>(app,
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

    std::vector<std::shared_ptr<Layout::Options::OptionsItem>> menu_input_options;
    std::vector<std::shared_ptr<Layout::Options::OptionsItem>> game_input_options;
    {
        menu_input_options.emplace_back(std::make_shared<DeviceChooser>(app,
            tr("Device"),
            [this, &app](DeviceID device_id){
                current_device_id = device_id;
                assert(input_device_panels.count(device_id));
                // clear the menu items, except the device selector,
                // and create the input fields for the current device
                auto& menu_panel = subitem_panels.at(2); // TODO: fix magic numbers
                menu_panel.erase(menu_panel.begin() + 1, menu_panel.end());
                for (const auto& item : input_device_panels.at(device_id).menu)
                    menu_panel.push_back(item);
                // the same for the game inputs
                auto& game_panel = subitem_panels.at(3); // TODO: fix magic numbers
                game_panel.erase(game_panel.begin() + 1, game_panel.end());
                for (const auto& item : input_device_panels.at(device_id).game)
                    game_panel.push_back(item);
                // finally, set the positions of the ui elements
                updatePositions(app.gcx());
            }));
        menu_input_options.back()->setMarginBottom(30);
        game_input_options.emplace_back(menu_input_options.front());

        const auto& devices = app.window().connectedDevices();
        for (const auto& device : devices) {
            static const std::vector<std::pair<std::string, InputType>> menu_eventnames = {
                {tr("Up"), InputType::MENU_UP},
                {tr("Down"), InputType::MENU_DOWN},
                {tr("Left"), InputType::MENU_LEFT},
                {tr("Right"), InputType::MENU_RIGHT},
                {tr("OK/Select"), InputType::MENU_OK},
                {tr("Cancel/Back"), InputType::MENU_CANCEL},
            };
            static const std::vector<std::pair<std::string, InputType>> game_eventnames = {
                {tr("Move left"), InputType::GAME_MOVE_LEFT},
                {tr("Move right"), InputType::GAME_MOVE_RIGHT},
                {tr("Rotate clockwise"), InputType::GAME_ROTATE_RIGHT},
                {tr("Rotate counter-clockwise"), InputType::GAME_ROTATE_LEFT},
                {tr("Soft drop"), InputType::GAME_SOFTDROP},
                {tr("Hard drop"), InputType::GAME_HARDDROP},
                {tr("Hold"), InputType::GAME_HOLD},
                {tr("Pause"), InputType::GAME_PAUSE},
            };

            auto& panel = input_device_panels[device.second.id];
            panel.menu = createInputFieldsForEvents(app, device.second, menu_eventnames);
            panel.game = createInputFieldsForEvents(app, device.second, game_eventnames);
        }

        //for (const auto& item : input_device_panels.at(current_device_id))
        //    input_options.push_back(item);
        auto& default_panel = input_device_panels.at(current_device_id);
        menu_input_options.insert(menu_input_options.end(), default_panel.menu.begin(), default_panel.menu.end());
        game_input_options.insert(game_input_options.end(), default_panel.game.begin(), default_panel.game.end());
    }
    subitem_panels.push_back(std::move(menu_input_options));
    subitem_panels.push_back(std::move(game_input_options));


    updatePositions(app.gcx());

    fn_category_input = [this, &parent, &app](InputType input){
        assert(!current_subitem);
        switch (input) {
            case InputType::MENU_OK:
                current_input_handler = &fn_settings_input;
                current_subitem = subitem_panels.at(current_category_idx).front().get();
                current_setting_idx = 0;
                current_subitem->onHoverEnter();
                break;
            case InputType::MENU_CANCEL:
                current_input_handler = nullptr;
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
    fn_settings_input = [this, &app](InputType input){
        assert(current_subitem);
        if (current_subitem->isLocked()) {
            current_subitem->onPress(app, input);
            return;
        }
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
            default:
                current_subitem->onPress(app, input);
                break;
        }
    };
    current_input_handler = &fn_category_input;
}

Options::~Options() = default;

std::vector<std::shared_ptr<Layout::Options::OptionsItem>>
Options::createInputFieldsForEvents(AppContext& app, const DeviceData& device_data,
                                         const std::vector<std::pair<std::string, InputType>>& event_names)
{
    std::vector<std::shared_ptr<Layout::Options::OptionsItem>> input_fields;
    for (const auto& eventname : event_names) {
        // TODO: Fix these!
        assert(device_data.eventmap.count(eventname.second));
        assert(!device_data.eventmap.at(eventname.second).empty());
        input_fields.emplace_back(std::make_shared<Layout::Options::InputField>(app,
            std::string(eventname.first),
            device_data.id, device_data.eventmap.at(eventname.second).front(),
            [this, &app, eventname](RawInputEvent raw_input) {
                app.window().setKeyBinding(current_device_id, eventname.second, raw_input.button);
            }
        ));
    }
    return input_fields;
}

std::vector<std::string> Options::detectedThemes() const
{
    std::vector<std::string> found_themes;
    findThemeDirs(Paths::config() + "themes", found_themes);
    findThemeDirs(Paths::data() + "themes", found_themes);
    assert(!found_themes.empty());
    return found_themes;
}

void Options::findThemeDirs(const std::string& base_path, std::vector<std::string>& found_themes) const
{
    if (!path_exists(base_path))
        return;

    TinyDir base_theme_dir(base_path);
    while (base_theme_dir.dir.has_next) {
        tinydir_file file;
        tinydir_readfile(&base_theme_dir.dir, &file);

        const std::string cfg_path = std::string(file.path) + "/theme.cfg";
        if (file.is_dir && path_exists(cfg_path))
            found_themes.emplace_back(file.name);

        tinydir_next(&base_theme_dir.dir);
    }
}

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

    if (current_subitem && current_subitem->isLocked()) {
        for (const auto& event : events) {
            switch (event.type) {
                case EventType::INPUT_RAW:
                    if (event.raw_input.is_down) {
                        current_subitem->onRawPress(app, event.raw_input);
                        // do not parse regular events in the frame where the key binding changed
                        if (!current_subitem->isLocked())
                            return;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    for (const auto& event : events) {
        assert(current_input_handler);
        switch (event.type) {
            case EventType::INPUT:
                if (event.input.down())
                    (*current_input_handler)(event.input.type());
                break;
            default:
                break;
        }
        if (!current_input_handler) {
            GameConfigFile::save(app.sysconfig(), app.wellconfig(), Paths::config() + "game.cfg");
            assert(parent.states.size() > 1);
            parent.states.pop_back();
            return;
        }
    }
}

void Options::draw(MainMenuState& parent, GraphicsContext& gcx) const
{
    parent.states.front()->draw(parent, gcx);

    RGBAColor black = 0x80_rgba;
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
        if (description_tex) {
            auto bgrect = container_rect;
            bgrect.h = description_tex->height() + 10;
            bgrect.y += container_rect.h - bgrect.h;
            gcx.drawFilledRect(bgrect, black);
            description_tex->drawAt(bgrect.x + 20, bgrect.y + 5);
        }
    }
}

} // namespace MainMenu
} // namespace SubStates
