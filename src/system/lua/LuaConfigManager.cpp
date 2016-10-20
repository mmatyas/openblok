#include "LuaConfigManager.h"

#include "system/Log.h"

#include "sol.hpp"
#include "SDL2/SDL_scancode.h" // TODO: fix this


const std::string LOG_TAG("config");

using InputScancodeMap = ConfigManager::InputScancodeMap;


InputScancodeMap LuaConfigManager::loadInputMapping(const std::string& scriptfile)
{
    static const InputScancodeMap default_map = {
        {InputType::GAME_PAUSE, {SDL_SCANCODE_P}},
        {InputType::GAME_HOLD, {SDL_SCANCODE_C, SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT}},
        {InputType::GAME_HARDDROP, {SDL_SCANCODE_UP, SDL_SCANCODE_SPACE}},
        {InputType::GAME_SOFTDROP, {SDL_SCANCODE_DOWN}},
        {InputType::GAME_MOVE_LEFT, {SDL_SCANCODE_LEFT}},
        {InputType::GAME_MOVE_RIGHT, {SDL_SCANCODE_RIGHT}},
        {InputType::GAME_ROTATE_LEFT, {SDL_SCANCODE_Z}},
        {InputType::GAME_ROTATE_RIGHT, {SDL_SCANCODE_X}},
    };

    static const std::vector<std::pair<InputType, std::string>> name_map = {
        {InputType::GAME_PAUSE, "pause"},
        {InputType::GAME_HOLD, "hold"},
        {InputType::GAME_HARDDROP, "hard_drop"},
        {InputType::GAME_SOFTDROP, "move_down"},
        {InputType::GAME_MOVE_LEFT, "move_left"},
        {InputType::GAME_MOVE_RIGHT, "move_right"},
        {InputType::GAME_ROTATE_LEFT, "rotate_left"},
        {InputType::GAME_ROTATE_RIGHT, "rotate_right"},
    };

    InputScancodeMap out = default_map;
    try {
        sol::state lua;
        lua.script_file(scriptfile);
        sol::table game = lua["game"];
        if (!game.valid())
            throw std::runtime_error(scriptfile + ": No 'game' block defined, using defaults.");

        for (const auto& key : name_map) {
            sol::object arr_or_num = game[key.second];
            if (!arr_or_num.valid())
                continue;

            out[key.first].clear();
            if (arr_or_num.get_type() == sol::type::table) {
                sol::table arr = arr_or_num;
                for (unsigned i = 1; i <= arr.size(); i++) {
                    sol::optional<uint16_t> val = arr[i];
                    if (val != sol::nullopt)
                        out[key.first].emplace(val.value());
                }
            }
            else if (arr_or_num.get_type() == sol::type::number){
                uint16_t val = arr_or_num.as<uint16_t>();
                out[key.first].emplace(val);
            }

            std::string dbg = key.second + ": ";
            for (const auto& elem : out.at(key.first))
                dbg += std::to_string(elem) + " ";
            Log::info(LOG_TAG) << dbg << "\n";
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

void LuaConfigManager::saveInputMapping(const InputScancodeMap& mapping, const std::string& scriptfile)
{
}
