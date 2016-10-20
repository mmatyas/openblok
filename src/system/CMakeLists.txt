set(MOD_SYSTEM_SRC
	Color.cpp
	ConfigManager.cpp
	InputEvent.cpp
	Log.cpp
	Window.cpp

	# SDL2
	sdl/SDLAudioContext.cpp
	sdl/SDLFont.cpp
	sdl/SDLGraphicsContext.cpp
	sdl/SDLMusic.cpp
	sdl/SDLSoundEffect.cpp
	sdl/SDLTexture.cpp
	sdl/SDLWindow.cpp

	# Lua
	lua/LuaConfigManager.cpp
)

set(MOD_SYSTEM_H
	AudioContext.h
	Color.h
	ConfigManager.h
	Font.h
	GraphicsContext.h
	InputEvent.h
	Localize.h
	Log.h
	Music.h
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

	# Lua
	lua/LuaConfigManager.h
)

add_library(module_system ${MOD_SYSTEM_SRC} ${MOD_SYSTEM_H})

# use SDL2
include_directories(${SDL2PP_INCLUDE_DIRS})

# use Lua
include_directories(${LUAJIT_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/external/sol)

target_link_libraries(module_system ${SDL2PP_LIBRARIES})
target_link_libraries(module_system ${LUAJIT_LIBRARIES})
