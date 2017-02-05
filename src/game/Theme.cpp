#include "Theme.h"

#include "system/ConfigFile.h"
#include "system/Log.h"
#include "system/Paths.h"

#include <regex>
#include <set>


// TODO: this file has some copies from GameConfigFile

ThemeConfig::Subdirs::Subdirs()
    : root(Paths::data() + "../themes/default")
{}

std::string ThemeConfig::Subdirs::background() {
    static const std::string dir = "/backgrounds/";
    return root + dir;
}
std::string ThemeConfig::Subdirs::music() {
    static const std::string dir = "/music/";
    return root + dir;
}
std::string ThemeConfig::Subdirs::sfx() {
    static const std::string dir = "/sfx/";
    return root + dir;
}
std::string ThemeConfig::Subdirs::graphics() {
    static const std::string dir = "/";
    return root + dir;
}

const std::string LOG_TAG("themecfg");

std::unordered_map<std::string, bool*> createBoolBinds(ThemeConfig& theme) {
    return {
        {"draw_well", &theme.gameplay.draw_well},
        {"draw_text", &theme.gameplay.draw_text},
        {"draw_panels", &theme.gameplay.draw_panels},
    };
}

ThemeConfig ThemeConfigFile::load(const std::string& dir)
{
    const std::string path = dir + "/theme.cfg";
    const auto& config = ConfigFile::load(path);
    if (config.empty())
        return {};

    const std::regex valid_value(R"(([0-9]{1,3}|on|off|yes|no|true|false|[a-z]+))");
    const std::set<std::string> accepted_headers = {"meta", "gameplay"};

    ThemeConfig tcfg;
    tcfg.dirs.root = Paths::data() + "../themes" + dir; // TODO: this is kinda ugly

    auto bool_binds = createBoolBinds(tcfg);

    for (const auto& block : config) {
        const auto& block_name = block.first;
        if (!accepted_headers.count(block_name)) {
            Log::warning(LOG_TAG) << path << ": Unknown settings block '" << block_name << "', skipped\n";
            continue;
        }

        for (const auto& keyval : block.second) {
            const auto& key_str = keyval.first;
            const auto& val_str = keyval.second;

            if (!std::regex_match(val_str, valid_value)) {
                Log::warning(LOG_TAG) << path << ": Unknown value '" << val_str
                                              << "' for '" << key_str << "'\n";
                Log::warning(LOG_TAG) << path << ": under '" << block_name << "', skipped\n";
                continue;
            }

            try {
                if (bool_binds.count(key_str))
                    *bool_binds.at(key_str) = ConfigFile::parseBool(keyval);
                else
                    throw std::runtime_error("Unknown option '" + key_str + "', ignored");
            }
            catch (const std::runtime_error& err) {
                Log::warning(LOG_TAG) << path << ": " << err.what() << "\n";
                Log::warning(LOG_TAG) << path << ": under '" << block_name << "'\n";
            }
        }
    }

    return tcfg;
}
