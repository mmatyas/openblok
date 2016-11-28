#include "GameConfigFile.h"

#include "system/Log.h"

#include <fstream>
#include <functional>
#include <regex>
#include <set>
#include <unordered_map>
#include <assert.h>


const std::string LOG_TAG("config");

std::unordered_map<std::string, bool*> createBoolBind(SysConfig& sys)
{
    return {
        {"fullscreen", &sys.fullscreen},
        {"sfx", &sys.sfx},
        {"music", &sys.music},
    };
}
std::unordered_map<std::string, bool*> createBoolBind(WellConfig& well)
{
    return {
        {"instant_harddrop", &well.instant_harddrop},
        {"tspin_enabled", &well.tspin_enabled},
        {"tspin_allow_wall", &well.tspin_allow_wallblock},
        {"tspin_allow_kick", &well.tspin_allow_wallkick},
    };
}
std::unordered_map<std::string, unsigned short*> createNumericBind(WellConfig& well)
{
    return {
        {"das_start", &well.shift_normal},
        {"das_repeat", &well.shift_turbo},
        {"lock_delay", &well.lock_delay},
    };
}

const std::set<std::string> accepted_wellenum_keys = {"lock_type", "rotation"};
const std::unordered_map<std::string, LockDelayType> str_to_locktype {
    {"\"instant\"", LockDelayType::CLASSIC},
    {"\"extended\"", LockDelayType::EXTENDED},
    {"\"infinite\"", LockDelayType::INFINITE},
};
const std::unordered_map<std::string, RotationStyle> str_to_rotation {
    {"\"srs\"", RotationStyle::SRS},
    {"\"tgm\"", RotationStyle::TGM},
    {"\"classic\"", RotationStyle::CLASSIC},
};

void writeBool(std::ofstream& ofs, const std::string& key, bool value)
{
    ofs << key << " = " << (value ? "on" : "off") << "\n";
}

void parseBool(bool& config_val, const std::string& config_key, const std::string& str)
{
    if (str == "on")
        config_val = true;
    else if (str == "off")
        config_val = false;
    else {
        throw std::runtime_error(
            "Invalid value for '" + config_key+ "', " +
            "expected 'on' or 'off', got '" + str + "'");
    }
}

void GameConfigFile::save(SysConfig& sys, WellConfig& well, const std::string& path)
{
    std::ofstream out(path);
    if (!out.is_open()) {
        Log::warning(LOG_TAG) << "Could not save game config to " << path << "\n";
        return;
    }

    out << "[system]\n";
    {
        auto sys_bools = createBoolBind(sys);
        for (const auto& pair : sys_bools)
            writeBool(out, pair.first, *pair.second);
    }
    out << "\n";
    out << "[gameplay]\n";
    {
        auto well_bools = createBoolBind(well);
        for (const auto& pair : well_bools)
            writeBool(out, pair.first, *pair.second);

        auto well_ushorts = createNumericBind(well);
        for (const auto& pair : well_ushorts)
            out << pair.first << " = " << *pair.second << "\n";

        std::map<LockDelayType, const std::string> locktype_to_str;
        for (const auto& pair : str_to_locktype)
            locktype_to_str.emplace(pair.second, pair.first);
        assert(locktype_to_str.count(well.lock_delay_type));
        out << "lock_type = " << locktype_to_str.at(well.lock_delay_type) << "\n";

        std::map<RotationStyle, const std::string> rotation_to_str;
        for (const auto& pair : str_to_rotation)
            rotation_to_str.emplace(pair.second, pair.first);
        assert(rotation_to_str.count(well.rotation_style));
        out << "rotation = " << rotation_to_str.at(well.rotation_style) << "\n";
    }
}

std::tuple<SysConfig, WellConfig> GameConfigFile::load(const std::string& path)
{
    const std::regex valid_head(R"(\[[a-zA-Z0-9\.-_]+\])");
    const std::regex valid_data(R"([a-z_]+\s*=\s*([0-9]{1,3}|on|off|"[a-z]+"))");
    const std::regex whitespace(R"(\s+)");

    SysConfig sys;
    WellConfig well;

    const std::vector<std::string> accepted_headers = {"system", "gameplay"};
    uint8_t current_header_idx = 0xFF;

    auto sys_bools = createBoolBind(sys);
    auto well_bools = createBoolBind(well);
    auto well_ushorts = createNumericBind(well);

    std::ifstream infile(path);
    unsigned linenum = 0;
    std::string line;
    while (std::getline(infile, line)) {
        linenum++;
        if (line.empty() || line.front() == '#')
            continue;

        if (std::regex_match(line, valid_head)) {
            auto current_head = line.substr(1, line.length() - 2);
            assert(!current_head.empty()); // the regex requires at least one char
            for (unsigned i = 0; i < accepted_headers.size(); i++) {
                if (accepted_headers.at(i) == current_head) {
                    current_header_idx = i;
                    break;
                }
            }
        }
        else if (std::regex_match(line, valid_data)) {
            if (current_header_idx == 0xFF) {
                Log::warning(LOG_TAG) << path << ":" << linenum << ": No known section header defined before this line\n";
                Log::warning(LOG_TAG) << "Using default settings\n";
                return std::make_tuple<SysConfig, WellConfig>({}, {});
            }

            line = std::regex_replace(line, whitespace, "");
            const auto split_pos = line.find("=");
            const auto key = line.substr(0, split_pos);
            const auto str_val = line.substr(split_pos + 1);


            if (sys_bools.count(key)) {
                try { parseBool(*sys_bools.at(key), key, str_val); }
                catch (const std::runtime_error& err) {
                    Log::warning(LOG_TAG) << path << ":" << linenum << ": " << err.what() << "\n";
                }
            }
            else if (well_bools.count(key)) {
                try { parseBool(*well_bools.at(key), key, str_val); }
                catch (const std::runtime_error& err) {
                    Log::warning(LOG_TAG) << path << ":" << linenum << ": " << err.what() << "\n";
                }
            }
            else if (well_ushorts.count(key)) {
                try {
                    auto value = std::stoul(str_val);
                    if (value > 0xFFFF)
                        throw std::out_of_range("");

                    *well_ushorts.at(key) = value;
                }
                catch (...) {
                    Log::warning(LOG_TAG) << path << ":" << linenum
                        << ": Invalid numeric value '" << str_val << "', ignored\n";
                }
            }
            else if (accepted_wellenum_keys.count(key)) {
                if (key == "lock_type") {
                    if (str_to_locktype.count(str_val))
                        well.lock_delay_type = str_to_locktype.at(str_val);
                    else {
                        Log::warning(LOG_TAG) << path << ":" << linenum
                        << ": Invalid lock type value '" << str_val << "', ignored\n";
                    }
                }
                else if (key == "rotation") {
                    if (str_to_rotation.count(str_val))
                        well.rotation_style = str_to_rotation.at(str_val);
                    else {
                        Log::warning(LOG_TAG) << path << ":" << linenum
                        << ": Invalid rotation style value '" << str_val << "', ignored\n";
                    }
                }
            }
            else {
                Log::warning(LOG_TAG) << path << ":" << linenum
                << ": Unknown option '" << key << "' for section '"
                << accepted_headers.at(current_header_idx) << "', ignored\n";
            }
        }
        else {
            Log::warning(LOG_TAG) << path << ":" << linenum << ": Syntax error\n" << line;
            Log::warning(LOG_TAG) << "Using default settings\n";
            return std::make_tuple<SysConfig, WellConfig>({}, {});
        }
    }

    return std::make_tuple<SysConfig, WellConfig>(std::move(sys), std::move(well));
}

