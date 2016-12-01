#include "InputConfigFile.h"

#include "ConfigFile.h"
#include "Log.h"

#include <assert.h>
#include <regex>
#include <sstream>


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

DeviceMap InputConfigFile::load(const std::string& path)
{
    const auto& config = ConfigFile::load(path);
    if (config.empty())
        return {};

    DeviceMap output;

    const std::regex valid_head_keyboard(R"(^keyboard$)");
    const std::regex valid_head_gamepad(R"(^G:)");
    const std::regex valid_head_joystick(R"(^J:)");
    const std::regex valid_button_list(R"([0-9]{1,3}(,[0-9]{1,3})*)");

    DeviceType current_device_type;

    for (const auto& block : config) {
        const auto& device_name = block.first;
        if (std::regex_match(device_name, valid_head_keyboard))
            current_device_type = DeviceType::KEYBOARD;
        else if (std::regex_match(device_name, valid_head_gamepad))
            current_device_type = DeviceType::GAMEPAD;
        else if (std::regex_match(device_name, valid_head_joystick))
            current_device_type = DeviceType::LEGACY_JOYSTICK;
        else {
            Log::warning(LOG_TAG) << path << ": Unknown device '" << device_name << "', skipped\n";
            continue;
        }

        auto& device = output[device_name];
        device.type = current_device_type;

        for (const auto& keyval : block.second) {
            const auto& key_str = keyval.first;
            const auto& val_str = keyval.second;

            if (!name_to_key.count(key_str)) {
                Log::warning(LOG_TAG) << path << ": Unknown input type '" << key_str
                                              << "' under '" << device_name << "', skipped\n";
                continue;
            }

            auto& codelist = device.eventmap[name_to_key.at(key_str)];
            codelist.clear();

            std::stringstream val_sstream(val_str);
            std::string number_str;
            while (std::getline(val_sstream, number_str, ',')) {
                try {
                    uint16_t value = std::stoul(number_str);
                    if (value > 0x200) // there can be 512 keys at most on a device
                        throw std::out_of_range("");
                    codelist.emplace_back(value);
                }
                catch (...) {
                    Log::warning(LOG_TAG) << path << ": Invalid value '" << number_str
                                                  << "' for '" << key_str << "'\n";
                    Log::warning(LOG_TAG) << path << ": under '" << device_name << "', skipped\n";
                }
            }
            // remove duplicates and sort
            std::sort(codelist.begin(), codelist.end());
            codelist.erase(std::unique(codelist.begin(), codelist.end()), codelist.end());
        }
    }
    return output;
}

void InputConfigFile::save(const DeviceMap& device_maps, const std::string& path)
{
    std::map<InputType, const std::string> key_to_name;
    for (const auto& pair : name_to_key)
        key_to_name.emplace(pair.second, pair.first);

    ConfigFile::Data config;

    for (const auto& device : device_maps) {
        const auto& device_type = device.second.type;
        const auto& event_map = device.second.eventmap;

        ConfigFile::BlockName device_name = device.first;
        switch (device_type) {
            case DeviceType::GAMEPAD:
                device_name = "G:" + device_name;
                break;
            case DeviceType::LEGACY_JOYSTICK:
                device_name = "J:" + device_name;
                break;
            default:
                break;
        }

        ConfigFile::KeyValPairs entries;
        for (const auto& elem : event_map) {
            // this puts commas between the numbers only
            std::ostringstream ss;
            std::copy(elem.second.begin(), elem.second.end() - 1, std::ostream_iterator<int>(ss, ", "));
            ss << elem.second.back();

            assert(key_to_name.count(elem.first));
            entries.emplace(key_to_name.at(elem.first), ss.str());
        }

        config.emplace(std::move(device_name), std::move(entries));
    }

    ConfigFile::save(config, path);
}
