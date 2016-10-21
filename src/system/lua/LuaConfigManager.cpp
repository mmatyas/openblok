#include "LuaConfigManager.h"

#include "system/Log.h"

#include "sol.hpp"
#include "SDL2/SDL_scancode.h" // TODO: fix this
#include <algorithm>
#include <fstream>
#include <sstream>


using ScancodeMap = ConfigManager::ScancodeMap;

const std::string LOG_TAG("config");
const std::map<InputType, const std::string> name_map = {
    {InputType::GAME_PAUSE, "pause"},
    {InputType::GAME_HOLD, "hold"},
    {InputType::GAME_HARDDROP, "hard_drop"},
    {InputType::GAME_SOFTDROP, "move_down"},
    {InputType::GAME_MOVE_LEFT, "move_left"},
    {InputType::GAME_MOVE_RIGHT, "move_right"},
    {InputType::GAME_ROTATE_LEFT, "rotate_left"},
    {InputType::GAME_ROTATE_RIGHT, "rotate_right"},
};

ScancodeMap LuaConfigManager::loadInputMapping(const std::string& scriptfile)
{
    static const ScancodeMap default_map = {
        {InputType::GAME_PAUSE, {SDL_SCANCODE_P}},
        {InputType::GAME_HOLD, {SDL_SCANCODE_C, SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT}},
        {InputType::GAME_HARDDROP, {SDL_SCANCODE_UP, SDL_SCANCODE_SPACE}},
        {InputType::GAME_SOFTDROP, {SDL_SCANCODE_DOWN}},
        {InputType::GAME_MOVE_LEFT, {SDL_SCANCODE_LEFT}},
        {InputType::GAME_MOVE_RIGHT, {SDL_SCANCODE_RIGHT}},
        {InputType::GAME_ROTATE_LEFT, {SDL_SCANCODE_Z}},
        {InputType::GAME_ROTATE_RIGHT, {SDL_SCANCODE_X}},
    };

    ScancodeMap out = default_map;
    try {
        sol::state lua;
        lua.script_file(scriptfile);
        sol::table game = lua["game"];
        if (!game.valid())
            throw std::runtime_error(scriptfile + ": No 'game' block defined, using defaults.");

        sol::table keyboard = game["keyboard"];
        if (!keyboard.valid())
            throw std::runtime_error(scriptfile + ": No 'keyboard' block defined, using defaults.");

        for (const auto& key : name_map) {
            sol::object arr_or_num = keyboard[key.second];
            if (!arr_or_num.valid())
                continue;

            auto& scancodes = out[key.first];
            scancodes.clear();

            // an array
            if (arr_or_num.get_type() == sol::type::table) {
                sol::table arr = arr_or_num;
                for (unsigned i = 1; i <= arr.size(); i++) {
                    sol::optional<uint16_t> val = arr[i];
                    if (val != sol::nullopt)
                        scancodes.emplace_back(val.value());
                }

                // remove duplicates and sort
                std::sort(scancodes.begin(), scancodes.end());
                scancodes.erase(std::unique(scancodes.begin(), scancodes.end()), scancodes.end());
            }
            // one number
            else if (arr_or_num.get_type() == sol::type::number){
                uint16_t val = arr_or_num.as<uint16_t>();
                scancodes.emplace_back(val);
            }
        }
    }
    catch (const sol::error& err) {
        Log::warning(LOG_TAG) << err.what() << '\n';
    }
    catch (const std::runtime_error& err) {
        Log::warning(LOG_TAG) << err.what() << '\n';
    }

    return out;
}

void LuaConfigManager::saveInputMapping(const ScancodeMap& mapping, const std::string& scriptfile)
{
    std::ofstream out(scriptfile);
    if (!out.is_open()) {
        Log::warning(LOG_TAG) << "Could not save input config to " << scriptfile << "\n";
        return;
    }


    out << "game = {\n";
    out << "\tkeyboard = {\n";
    for (const auto& elem : mapping) {
        assert(name_map.count(elem.first));

        out << "\t\t" << name_map.at(elem.first) << " = ";
        if (elem.second.size() == 1) {
            out << *elem.second.begin() << ",\n";
        }
        else {
            // this puts commas between the numbers only
            std::ostringstream ss;
            std::copy(elem.second.begin(), elem.second.end() - 1, std::ostream_iterator<int>(ss, ", "));
            ss << elem.second.back();
            out << "{" << ss.str() << "},\n";
        }
    }
    out << "\t}\n";
    out << "}\n";
}
