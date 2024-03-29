set(MOD_SYSTEM_SRC
    Color.cpp
    ConfigFile.cpp
    Event.cpp
    InputConfigFile.cpp
    InputMap.cpp
    Localize.cpp
    Log.cpp
    Paths.cpp
    Window.cpp

    # SDL2
    sdl/SDLAudioContext.cpp
    sdl/SDLFont.cpp
    sdl/SDLGraphicsContext.cpp
    sdl/SDLMusic.cpp
    sdl/SDLSoundEffect.cpp
    sdl/SDLTexture.cpp
    sdl/SDLWindow.cpp
)

set(MOD_SYSTEM_H
    AudioContext.h
    Color.h
    ConfigFile.h
    Event.h
    Font.h
    GraphicsContext.h
    InputMap.h
    InputConfigFile.h
    Localize.h
    Log.h
    Music.h
    Paths.h
    Rectangle.h
    SoundEffect.h
    Texture.h
    Window.h

    # SDL2
    sdl/SDLAudioContext.h
    sdl/SDLFont.h
    sdl/SDLGraphicsContext.h
    sdl/SDLMusic.h
    sdl/SDLSoundEffect.h
    sdl/SDLTexture.h
    sdl/SDLWindow.h

    # Util
    util/MakeUnique.h
)

add_library(module_system ${MOD_SYSTEM_SRC} ${MOD_SYSTEM_H})
target_include_directories(module_system PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../generated")

# use SDL2
target_compile_definitions(module_system PRIVATE OPENBLOK_DATADIR="${DATADIR}")
target_link_libraries(module_system SDL2pp)

# GNU gettext
if(ENABLE_LOCALES)
    find_package(Intl REQUIRED)
    find_package(Iconv REQUIRED)

    target_include_directories(module_system PRIVATE ${Intl_INCLUDE_DIRS})
    target_link_libraries(module_system ${Intl_LIBRARIES} Iconv::Iconv)

    target_compile_definitions(module_system PUBLIC  WITH_LOCALES)
endif()

if(VITA)
    target_link_libraries(module_system
        SDL2
        webp
        pthread
        png
        freetype
        vorbisfile
        vorbis
        ogg
        jpeg
        z
        mikmod
        modplug
        mpg123
        FLAC
        SceAudio_stub
        SceAudioIn_stub
        SceCommonDialog_stub
        SceCtrl_stub
        SceDisplay_stub
        SceGxm_stub
        SceHid_stub
        SceTouch_stub
        SceMotion_stub
    )
endif()

include(EnableWarnings)
include(RequireCxx11)
enable_warnings(module_system)
require_cxx11_or_higher(module_system)

option(ENABLE_JPG "Enable JPG image support" ON)
option(ENABLE_MP3 "Enable MP3 music support" ON)
option(ENABLE_MOD "Enable MOD music support" OFF)
option(ENABLE_FLAC "Enable FLAC music support" OFF)

if (ENABLE_JPG)
    target_compile_definitions(module_system PRIVATE SDL2_ENABLE_JPG)
endif()
if (ENABLE_MP3)
    target_compile_definitions(module_system PRIVATE SDL2_ENABLE_MP3)
endif()
if (ENABLE_MOD)
    target_compile_definitions(module_system PRIVATE SDL2_ENABLE_MOD)
endif()
if (ENABLE_FLAC)
    target_compile_definitions(module_system PRIVATE SDL2_ENABLE_FLAC)
endif()
