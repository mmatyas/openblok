#include "GameConfigFile.h"

#include "system/ConfigFile.h"
#include "system/Log.h"

#include <map>
#include <fstream>
#include <functional>
#include <regex>
#include <set>
#include <unordered_map>
#include <assert.h>


const std::string LOG_TAG("config");

std::unordered_map<std::string, bool*> createBoolBind(SysConfig& sys) {
    return {
        {"fullscreen", &sys.fullscreen},
        {"sfx", &sys.sfx},
        {"music", &sys.music},
    };
}
std::unordered_map<std::string, std::string*> createStringBind(SysConfig& sys) {
    return {
        {"theme", &sys.theme_dir_name},
    };
}
std::unordered_map<std::string, bool*> createBoolBind(WellConfig& well) {
    return {
        {"instant_harddrop", &well.instant_harddrop},
        {"tspin_enabled", &well.tspin_enabled},
        {"tspin_allow_wall", &well.tspin_allow_wallblock},
        {"tspin_allow_kick", &well.tspin_allow_wallkick},
    };
}
std::unordered_map<std::string, unsigned short*> createNumericBind(WellConfig& well) {
    return {
        {"das_start", &well.shift_normal},
        {"das_repeat", &well.shift_turbo},
        {"lock_delay", &well.lock_delay},
        {"max_next_pieces", &well.max_next_pieces},
    };
}

const std::set<std::string> accepted_wellenum_keys = {"lock_type", "rotation"};
const std::unordered_map<std::string, LockDelayType> str_to_locktype {
    {"instant", LockDelayType::CLASSIC},
    {"extended", LockDelayType::EXTENDED},
    {"infinite", LockDelayType::INFINITE},
};
const std::unordered_map<std::string, RotationStyle> str_to_rotation {
    {"srs", RotationStyle::SRS},
    {"tgm", RotationStyle::TGM},
    {"classic", RotationStyle::CLASSIC},
};

const std::string boolAsStr(bool value)
{
    return value ? "on" : "off";
}

void GameConfigFile::save(SysConfig& sys, WellConfig& well, const std::string& path)
{
    std::ofstream out(path);
    if (!out.is_open()) {
        Log::warning(LOG_TAG) << "Could not save game config to " << path << "\n";
        return;
    }

    ConfigFile::Blocks config;
    {
        ConfigFile::KeyValPairs sys_entries;
        auto sys_bools = createBoolBind(sys);
        for (const auto& pair : sys_bools)
            sys_entries.emplace(pair.first, boolAsStr(*pair.second));

        auto sys_strings = createStringBind(sys);
        for (const auto& pair : sys_strings)
            sys_entries.emplace(pair.first, '"' + *pair.second + '"');

        config.emplace("system", std::move(sys_entries));
    }
    {
        ConfigFile::KeyValPairs gameplay_entries;
        auto well_bools = createBoolBind(well);
        for (const auto& pair : well_bools)
            gameplay_entries.emplace(pair.first, boolAsStr(*pair.second));

        auto well_ushorts = createNumericBind(well);
        for (const auto& pair : well_ushorts)
            gameplay_entries.emplace(pair.first, std::to_string(*pair.second));

        std::map<LockDelayType, const std::string> locktype_to_str;
        for (const auto& pair : str_to_locktype)
            locktype_to_str.emplace(pair.second, pair.first);
        assert(locktype_to_str.count(well.lock_delay_type));
        gameplay_entries.emplace("lock_type", locktype_to_str.at(well.lock_delay_type));

        std::map<RotationStyle, const std::string> rotation_to_str;
        for (const auto& pair : str_to_rotation)
            rotation_to_str.emplace(pair.second, pair.first);
        assert(rotation_to_str.count(well.rotation_style));
        gameplay_entries.emplace("rotation", rotation_to_str.at(well.rotation_style));

        config.emplace("gameplay", std::move(gameplay_entries));
    }
    ConfigFile::save(config, path);
}

std::tuple<SysConfig, WellConfig> GameConfigFile::load(const std::string& path)
{
    const auto& config = ConfigFile::load(path);
    if (config.empty())
        return {};

    const std::regex valid_value(R"(([0-9]{1,3}|on|off|yes|no|true|false|[a-z]+|".+?"))");
    const std::set<std::string> accepted_headers = {"system", "gameplay"};

    SysConfig sys;
    WellConfig well;

    auto sys_bools = createBoolBind(sys);
    auto sys_strings = createStringBind(sys);
    auto well_bools = createBoolBind(well);
    auto well_ushorts = createNumericBind(well);

    for (const auto& block : config) {
        const auto& block_name = block.first;
        if (!accepted_headers.count(block_name)) {
            Log::warning(LOG_TAG) << path << ": Unknown settings block '" << block_name << "', skipped\n";
            continue;
        }

        for (const auto& keyval : block.second) {
            const auto& key_str = keyval.first;
            const auto& val_str = keyval.second;

            if (!regex_match(val_str, valid_value)) {
                Log::warning(LOG_TAG) << path << ": Unknown value '" << val_str
                                              << "' for '" << key_str << "'\n";
                Log::warning(LOG_TAG) << path << ": under '" << block_name << "', skipped\n";
                continue;
            }

            try {
                if (sys_bools.count(key_str)) {
                    *sys_bools.at(key_str) = ConfigFile::parseBool(keyval);
                }
                else if (sys_strings.count(key_str)) {
                    if (val_str.size() <= 2)
                        throw std::runtime_error("Empty string value for '" + key_str + "', skipped");
                    if (val_str.front() != '"' || val_str.back() != '"')
                        throw std::runtime_error("String value for '" + key_str + "', is not between quote marks, skipped");

                    *sys_strings.at(key_str) = val_str.substr(1, val_str.size() - 2);
                }
                else if (well_bools.count(key_str)) {
                    *well_bools.at(key_str) = ConfigFile::parseBool(keyval);
                }
                else if (well_ushorts.count(key_str)) {
                    try {
                        auto value = std::stoul(val_str);
                        if (value > 0xFFFF)
                            throw std::out_of_range("");

                        *well_ushorts.at(key_str) = value;
                    }
                    catch (...) {
                        throw std::runtime_error("Invalid numeric value '" + val_str + "', skipped");
                    }
                }
                else if (accepted_wellenum_keys.count(key_str)) {
                    if (key_str == "lock_type") {
                        if (str_to_locktype.count(val_str))
                            well.lock_delay_type = str_to_locktype.at(val_str);
                        else
                            throw std::runtime_error("Invalid lock type value '" + val_str + "', skipped");
                    }
                    else if (key_str == "rotation") {
                        if (str_to_rotation.count(val_str))
                            well.rotation_style = str_to_rotation.at(val_str);
                        else
                            throw std::runtime_error("Invalid rotation style value '" + val_str + "', skipped");
                    }
                }
                else
                    throw std::runtime_error("Unknown option '" + key_str + "', ignored");
            }
            catch (const std::runtime_error& err) {
                Log::warning(LOG_TAG) << path << ": " << err.what() << "\n";
                Log::warning(LOG_TAG) << path << ": under '" << block_name << "'\n";
            }
        }
    }

    return std::make_tuple<SysConfig, WellConfig>(std::move(sys), std::move(well));
}

