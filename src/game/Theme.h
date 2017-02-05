#pragma once

#include <string>


struct ThemeConfig {
    struct Subdirs {
        std::string root;
        std::string background();
        std::string music();
        std::string sfx();
        std::string graphics();

        Subdirs();
    } dirs;

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

    std::string random_menu_music();
    std::string random_game_music();
    std::string random_game_background();
};

class ThemeConfigFile {
public:
    static ThemeConfig load(const std::string& path);
};
