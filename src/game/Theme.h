#pragma once

#include <string>


struct ThemeConfig {
    struct GameplayConfig {
        bool draw_well;
        bool draw_text;
        bool draw_panels;

        GameplayConfig()
            : draw_well(true)
            , draw_text(true)
            , draw_panels(true)
        {}
    } gameplay;

    void set_theme_dir(const std::string&);

    std::string config_path() const { return m_config_path; }

    std::string random_menu_music() const;
    std::string random_game_music() const;
    std::string random_game_background() const;

    std::string get_sfx(const std::string&) const;
    std::string get_texture(const std::string&) const;

private:
    std::string m_theme_dir;
    std::string m_config_path;

    std::string resolve_path(const std::string&) const;
    std::string random_file_from(const std::string&) const;
};

class ThemeConfigFile {
public:
    static ThemeConfig load(const std::string& path);
};
