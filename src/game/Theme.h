#pragma once

#include "system/Color.h"

#include <string>


struct ThemeColors {
    RGBAColor label_normal;
    RGBAColor label_highlight;

    RGBAColor pause_normal;
    RGBAColor pause_highlight;

    RGBAColor mainmenu_normal;
    RGBAColor mainmenu_highlight;
    RGBAColor mainmenu_panel;

    RGBAColor countdown;
    RGBAColor game_end;
    RGBAColor popup;
    RGBAColor line_clear;

    ThemeColors()
        : label_normal(0xEEEEEEFF_rgba)
        , label_highlight(0xFFA500FF_rgba)
        , pause_normal(label_normal)
        , pause_highlight(label_highlight)
        , mainmenu_normal(0x006080FF_rgba)
        , mainmenu_highlight(label_normal)
        , mainmenu_panel(0x2030FFFF_rgba)
        , countdown(label_normal)
        , game_end(label_normal)
        , popup(label_normal)
        , line_clear(0xEEEEEEFF_rgba)
    {}
};

struct GameplayTheme {
    bool draw_labels;
    bool custom_minos;
    bool tint_ghost;

    GameplayTheme()
        : draw_labels(true)
        , custom_minos(false)
        , tint_ghost(true)
    {}
};

struct ThemeConfig {
    ThemeColors colors;
    GameplayTheme gameplay;

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
