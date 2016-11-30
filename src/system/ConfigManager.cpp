#include "ConfigManager.h"

#include "system/Log.h"

#include "SDL2/SDL_scancode.h" // TODO: fix this
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <assert.h>


const std::string LOG_TAG("config");

const std::map<const std::string, InputType> name_to_key = {
    {"pause", InputType::GAME_PAUSE},
    {"hold", InputType::GAME_HOLD},
    {"hard_drop", InputType::GAME_HARDDROP},
    {"move_down", InputType::GAME_SOFTDROP},
    {"move_left", InputType::GAME_MOVE_LEFT},
    {"move_right", InputType::GAME_MOVE_RIGHT},
    {"rotate_left", InputType::GAME_ROTATE_LEFT},
    {"rotate_right", InputType::GAME_ROTATE_RIGHT},
    {"menu_up", InputType::MENU_UP},
    {"menu_down", InputType::MENU_DOWN},
    {"menu_left", InputType::MENU_LEFT},
    {"menu_right", InputType::MENU_RIGHT},
    {"menu_ok", InputType::MENU_OK},
    {"menu_cancel", InputType::MENU_CANCEL},
};


DeviceMaps ConfigManager::loadInputMappings(const std::string& path)
{
    DeviceMaps default_map;
    default_map["keyboard"] = std::make_pair<DeviceType, EventToButtonsMap>(DeviceType::KEYBOARD, {
        {InputType::GAME_PAUSE, {SDL_SCANCODE_P}},
        {InputType::GAME_HOLD, {SDL_SCANCODE_C, SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT}},
        {InputType::GAME_HARDDROP, {SDL_SCANCODE_UP, SDL_SCANCODE_SPACE}},
        {InputType::GAME_SOFTDROP, {SDL_SCANCODE_DOWN}},
        {InputType::GAME_MOVE_LEFT, {SDL_SCANCODE_LEFT}},
        {InputType::GAME_MOVE_RIGHT, {SDL_SCANCODE_RIGHT}},
        {InputType::GAME_ROTATE_LEFT, {SDL_SCANCODE_Z}},
        {InputType::GAME_ROTATE_RIGHT, {SDL_SCANCODE_X}},
        {InputType::MENU_LEFT, {SDL_SCANCODE_LEFT}},
        {InputType::MENU_RIGHT, {SDL_SCANCODE_RIGHT}},
        {InputType::MENU_UP, {SDL_SCANCODE_UP}},
        {InputType::MENU_DOWN, {SDL_SCANCODE_DOWN}},
        {InputType::MENU_OK, {SDL_SCANCODE_RETURN, SDL_SCANCODE_SPACE, SDL_SCANCODE_Z}},
        {InputType::MENU_CANCEL, {SDL_SCANCODE_BACKSPACE, SDL_SCANCODE_ESCAPE, SDL_SCANCODE_X}},
    });
    DeviceMaps out = default_map;

    std::string line;
    std::string current_head_name;
    DeviceType current_head_type;
    const std::regex valid_head_keyboard(R"(\[[a-zA-Z0-9\.-_]+\])");
    const std::regex valid_head_gamejoy(R"(\[([GJ]:)?[a-zA-Z0-9\.-_]+\])");
    const std::regex valid_data(R"([a-z_]+\s*=\s*[0-9]{1,3}(,\s*[0-9]{1,3})*)");
    const std::regex whitespace(R"(\s+)");

    std::ifstream infile(path);
    unsigned linenum = 0;
    while (std::getline(infile, line)) {
        if (line.empty() || line.front() == '#')
            continue;

        if (std::regex_match(line, valid_head_keyboard)) {
            current_head_name = line.substr(1, line.length() - 2);
            current_head_type = DeviceType::KEYBOARD;
            assert(!current_head_name.empty()); // the regex requires at least one char
        }
        else if (std::regex_match(line, valid_head_gamejoy)) {
            current_head_name = line.substr(3, line.length() - 4);
            assert(!current_head_name.empty()); // the regex requires at least one char
            assert(line.at(1) == 'G' || line.at(1) == 'J'); // again, see the regex
            current_head_type = line.at(1) == 'G' ? DeviceType::GAMEPAD : DeviceType::LEGACY_JOYSTICK;
        }
        else if (std::regex_match(line, valid_data)) {
            if (current_head_name.empty()) {
                Log::warning(LOG_TAG) << path << ":" << linenum << ": No device defined before this line\n";
                Log::warning(LOG_TAG) << "Using default settings\n";
                return default_map;
            }

            std::regex_replace(line, whitespace, "");
            const std::size_t split_pos = line.find("=");
            const std::string key = line.substr(0, split_pos - 1);

            if (!name_to_key.count(key)) {
                Log::warning(LOG_TAG) << path << ":" << linenum
                    << ": Unknown input type '" << key << "', line ignored\n";
            }
            else {
                std::stringstream str_values(line.substr(split_pos + 1));
                std::string str_item;

                auto& device = out[current_head_name];
                device.first = current_head_type;
                auto& codelist = device.second[name_to_key.at(key)];
                codelist.clear();
                while (std::getline(str_values, str_item, ',')) {
                    try {
                        uint16_t value = std::stoul(str_item);
                        if (value > 0x200) // there can be 512 keys at most on a device
                            throw std::out_of_range("");

                        codelist.emplace_back(value);
                    }
                    catch (...) {
                        Log::warning(LOG_TAG) << path << ":" << linenum
                            << ": Invalid value '" << str_item << "', ignored\n";
                    }
                }
                // remove duplicates and sort
                std::sort(codelist.begin(), codelist.end());
                codelist.erase(std::unique(codelist.begin(), codelist.end()), codelist.end());
            }
        }
        else {
            Log::warning(LOG_TAG) << path << ":" << linenum << ": Syntax error\n";
            Log::warning(LOG_TAG) << "Using default settings\n";
            return default_map;
        }

        linenum++;
    }

    return out;
}

void ConfigManager::saveInputMapping(const DeviceMaps& mappings, const std::string& path)
{
    std::ofstream out(path);
    if (!out.is_open()) {
        Log::warning(LOG_TAG) << "Could not save input config to " << path << "\n";
        return;
    }

    std::map<InputType, const std::string> key_to_name;
    for (const auto& pair : name_to_key)
        key_to_name.emplace(pair.second, pair.first);


    for (const auto& device : mappings) {
        const auto& device_type = device.second.first;
        const auto& button_map = device.second.second;

        out << "[";
        switch (device_type) {
            case DeviceType::GAMEPAD: out << "G:"; break;
            case DeviceType::LEGACY_JOYSTICK: out << "J:"; break;
            default: break;
        }
        out << device.first << "]\n";

        for (const auto& elem : button_map) {
            assert(key_to_name.count(elem.first));

            out << key_to_name.at(elem.first) << " = ";
            // this puts commas between the numbers only
            std::ostringstream ss;
            std::copy(elem.second.begin(), elem.second.end() - 1, std::ostream_iterator<int>(ss, ", "));
            ss << elem.second.back();
            out << ss.str() << "\n";
        }
        out << "\n";
    }
}
