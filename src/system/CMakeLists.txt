set(MOD_SYSTEM_SRC
	Color.cpp
	InputEvent.cpp
	Log.cpp
	Window.cpp

	# SDL2
	sdl/SDLFont.cpp
	sdl/SDLGraphicsContext.cpp
	sdl/SDLTexture.cpp
	sdl/SDLWindow.cpp
)

set(MOD_SYSTEM_H
	Color.h
	Font.h
	GraphicsContext.h
	InputEvent.h
	Localize.h
	Log.h
	Rectangle.h
	Texture.h
	Window.h

	# SDL2
	sdl/SDLFont.h
	sdl/SDLGraphicsContext.h
	sdl/SDLTexture.h
	sdl/SDLWindow.h
)

add_library(module_system ${MOD_SYSTEM_SRC} ${MOD_SYSTEM_H})

# use SDL2
include_directories(${SDL2PP_INCLUDE_DIRS})
target_link_libraries(module_system ${SDL2PP_LIBRARIES} ${FONTCONFIG_LIBRARIES})
