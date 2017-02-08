#include "Theme.h"

#include "system/ConfigFile.h"
#include "system/Log.h"
#include "system/Paths.h"

#include <tinydir_cpp.h>
#include <regex>
#include <set>

#ifdef _MSC_VER
#include <io.h>
#define posix_access _access
#else
#include <unistd.h>
#define posix_access access
#endif


inline bool path_exists(const std::string& path) {
    return posix_access(path.c_str(), F_OK) == 0;
}


void ThemeConfig::set_theme_dir(const std::string& dir_name)
{
    m_theme_dir = "themes/" + dir_name + "/";
    m_config_path = resolve_path("theme.cfg");
}

std::string ThemeConfig::get_sfx(const std::string& filename) const {
    return resolve_path("sfx/" + filename);
}
std::string ThemeConfig::get_texture(const std::string& filename) const {
    return resolve_path("graphics/" + filename);
}

std::string ThemeConfig::resolve_path(const std::string& filename) const
{
    // 1. user local theme dir
    std::string path = Paths::config() + m_theme_dir + filename;
    if (path_exists(path))
        return path;

    // 2. install theme dir
    path = Paths::data() + m_theme_dir + filename;
    if (path_exists(path))
        return path;

    // 3. default
    return Paths::data() + "themes/default/" + filename;
}

std::string ThemeConfig::random_game_music() const
{
    const auto path = random_file_from("music/gameplay");
    return path.empty() ? (Paths::data() + "themes/default/music/gameplay/gameplay.ogg") : path;
}

std::string ThemeConfig::random_menu_music() const
{
    const auto path = random_file_from("music/menu");
    return path.empty() ? (Paths::data() + "themes/default/music/menu/menu.ogg") : path;
}

std::string ThemeConfig::random_game_background() const
{
    return random_file_from("backgrounds/gameplay");
}

std::string ThemeConfig::random_file_from(const std::string& dir_name) const
{
    const std::string base_path = resolve_path(dir_name);
    if (!path_exists(base_path))
        return "";

    TinyDir dir(base_path);
    const auto file_list = dir.fileList();
    if (file_list.empty())
        return "";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, file_list.size() - 1);
    return file_list.at(dis(gen));
}


// TODO: this file has some copies from GameConfigFile

const std::string LOG_TAG("themecfg");

std::unordered_map<std::string, bool*> createBoolBinds(ThemeConfig& theme) {
    return {
        {"draw_wellbg", &theme.gameplay.draw_wellbg},
        {"draw_labels", &theme.gameplay.draw_labels},
        {"draw_panels", &theme.gameplay.draw_panels},
    };
}

ThemeConfig ThemeConfigFile::load(const std::string& dir_name)
{
    ThemeConfig tcfg;
    tcfg.set_theme_dir(dir_name);

    const auto& config = ConfigFile::load(tcfg.config_path());
    if (config.empty())
        return tcfg;

    const std::regex valid_value(R"(([0-9]{1,3}|on|off|yes|no|true|false|[a-z]+))");
    const std::set<std::string> accepted_headers = {"meta", "gameplay"};


    auto bool_binds = createBoolBinds(tcfg);

    for (const auto& block : config) {
        const auto& block_name = block.first;
        if (!accepted_headers.count(block_name)) {
            Log::warning(LOG_TAG) << tcfg.config_path() << ": Unknown settings block '"
                                                        << block_name << "', skipped\n";
            continue;
        }

        if (block_name == "meta") // for now
            continue;

        for (const auto& keyval : block.second) {
            const auto& key_str = keyval.first;
            const auto& val_str = keyval.second;

            if (!std::regex_match(val_str, valid_value)) {
                Log::warning(LOG_TAG) << tcfg.config_path() << ": Unknown value '" << val_str
                                                            << "' for '" << key_str << "'\n";
                Log::warning(LOG_TAG) << tcfg.config_path() << ": under '" << block_name << "', skipped\n";
                continue;
            }

            try {
                if (bool_binds.count(key_str))
                    *bool_binds.at(key_str) = ConfigFile::parseBool(keyval);
                else
                    throw std::runtime_error("Unknown option '" + key_str + "', ignored");
            }
            catch (const std::runtime_error& err) {
                Log::warning(LOG_TAG) << tcfg.config_path() << ": " << err.what() << "\n";
                Log::warning(LOG_TAG) << tcfg.config_path() << ": under '" << block_name << "'\n";
            }
        }
    }

    return tcfg;
}
