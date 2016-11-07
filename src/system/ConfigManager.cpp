#include "ConfigManager.h"

#include "system/Log.h"

#include "SDL2/SDL_scancode.h" // TODO: fix this
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <assert.h>


using ScancodeMap = ConfigManager::ScancodeMap;
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


ScancodeMap ConfigManager::loadInputMapping(const std::string& path)
{
    const ScancodeMap default_map = {
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
        {InputType::MENU_CANCEL, {SDL_SCANCODE_ESCAPE, SDL_SCANCODE_X}},
    };

    ScancodeMap out = default_map;

    std::string line;
    std::string current_head;
    std::regex valid_head(R"(\[[a-zA-Z0-9\.-_]+\])");
    std::regex valid_data(R"([a-z_]+\s*=\s*[0-9]{1,3}(,\s*[0-9]{1,3})*)");
    std::regex whitespace(R"(\s+)");

    std::ifstream infile(path);
    unsigned linenum = 0;
    while (std::getline(infile, line)) {
        if (line.empty() || line.front() == '#')
            continue;

        if (std::regex_match(line, valid_head)) {
            current_head = line.substr(1, line.length() - 2);
        }
        else if (std::regex_match(line, valid_data)) {
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

                auto& codelist = out[name_to_key.at(key)];
                codelist.clear();
                while (std::getline(str_values, str_item, ',')) {
                    try {
                        uint16_t value = std::stoul(str_item);
                        if (value > 0x200)
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
            break;
        }

        linenum++;
    }

    return out;
}

void ConfigManager::saveInputMapping(const ScancodeMap& mapping, const std::string& path)
{
    std::ofstream out(path);
    if (!out.is_open()) {
        Log::warning(LOG_TAG) << "Could not save input config to " << path << "\n";
        return;
    }

    std::map<InputType, const std::string> key_to_name;
    for (const auto& pair : name_to_key)
        key_to_name.emplace(pair.second, pair.first);


    out << "[keyboard]\n";
    for (const auto& elem : mapping) {
        assert(key_to_name.count(elem.first));

        out << key_to_name.at(elem.first) << " = ";
        // this puts commas between the numbers only
        std::ostringstream ss;
        std::copy(elem.second.begin(), elem.second.end() - 1, std::ostream_iterator<int>(ss, ", "));
        ss << elem.second.back();
        out << ss.str() << "\n";
    }
}
