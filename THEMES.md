THEMES
======

You can customize OpenBlok by creating themes: instead of editing the data files directly, you can load your assets from a custom directory, or configure certain graphical options without the need of changing the source code.

To create a new theme, you have to create a new folder in the `themes` directory, create a `theme.cfg` file, then add your replacement files.


## The theme directory

Themes can be located in the following directories:

- `[CONFIGDIR]/themes`, where `[CONFIGDIR]` is
	- `~/.local/share/openblok` on Linux, and
	- `C:\Users\[USERNAME]\AppData\Roaming\openblok` on Windows.
- `[INSTALLDIR]/data/themes`, where `[INSTALLDIR]` is usually
	- `/usr/share/openblok` on Linux
	- `C\Program Files\openblok` on Windows
	- or where you've extracted the game


## The theme configuration

All theme needs a `theme.cfg` file, where you can list some basic information (title of the theme, the author's name, etc.), or change certain graphical settings (color scheme, visibility of UI elements, etc.).

Here is a full `theme.cfg` file with the default values as an example. Feel free to change or remove the existing parameters. All settings are optional: you don't have to list every existing option in the config file, missing parameters will use their the default values.

```python
# Lines starting with '#' are comments
# This is the 'metadata' block, where you
# can store info about the theme
[meta]
# Text values are between quote marks
author = "John Smith"
email = "jsmith@example.com"
description = "Some fancy theme"
version = "v1.2"

# This block defines the color scheme used in the game
# Accepted values are RGB or RGBA color in hex format
[colors]
# The color of most texts
text = #EEEEEE
# Color used for highlighting some texts
text_accent = #FFA500
# Color of UI panels and borders during gameplay
panel = #0A0AFF80
# The color of unselected elements in the main menu
mainmenu_inactive = #006080
# The color of themain menu button
mainmenu_selected = #2030FF
# The color of the line clear animation
line_clear = #EEEEEE

# Gameplay-related graphical settings
# For logical values, you can use yes/no/true/false/on/off
[gameplay]
# Draw the panels/borders?
draw_panels = yes
# Draw the labels like 'Next', 'Level', etc.?
draw_labels = yes
# Draw the background tiles of the well?
draw_wellbg = yes
# If this is off, all minos will have the same texture,
# but tinted with a different color
# If this is on, you'll need to have a texture file for all
# the different minos: mino_{l,r,s,z,t,i,o,garbage}.png
custom_minos = no
# Tint the ghost texture with the piece's color?
tint_ghost = yes
```


## The theme files

After creating the configuration file. you can your replacement files into the theme directory. The directory structure and the file names should match the original (default) theme.

```
yourtheme/
├── theme.cfg
├── graphics/
│   └── # basic graphical assets
├── music/
│   │   # songs will be selected randomly
│   │   # supported formats: OGG, FLAC, MP3
│   ├── menu/ # musics of the main menu
│   └── gameplay/ # musics of the gameplay
├── sfx/
│   └── # sound effects
│       # supported formats: WAV, OGG
└── backgrounds/
    │   # backgrounds will be selected randomly
    │   # supported formats: PNG, JPG
    └── gameplay/
```

If a file is not found, the one from the default theme will be used.
