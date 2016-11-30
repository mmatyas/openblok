set(MOD_SYSTEM_SRC
    Color.cpp
    ConfigFile.cpp
    Event.cpp
    InputConfigFile.cpp
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
)

add_library(module_system ${MOD_SYSTEM_SRC} ${MOD_SYSTEM_H})

# use SDL2
include_directories(${SDL2PP_INCLUDE_DIRS})
target_link_libraries(module_system ${SDL2PP_LIBRARIES})
