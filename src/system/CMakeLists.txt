set(MOD_SYSTEM_SRC
	Color.cpp
	FontManager.cpp
	InputEvent.cpp
	Log.cpp
	Window.cpp

	# SDL2
	sdl/SDLGraphicsContext.cpp
	sdl/SDLWindow.cpp

	# FontConfig
	fontconfig/FontConfig.cpp
)

set(MOD_SYSTEM_H
	Color.h
	FontManager.h
	GraphicsContext.h
	InputEvent.h
	Localize.h
	Log.h
	Window.h

	# SDL2
	sdl/SDLGraphicsContext.h
	sdl/SDLWindow.h

	# FontConfig
	fontconfig/FontConfig.h
)

add_library(module_system ${MOD_SYSTEM_SRC} ${MOD_SYSTEM_H})

# use SDL2
include_directories(${SDL2PP_INCLUDE_DIRS})
target_link_libraries(module_system ${SDL2PP_LIBRARIES} ${FONTCONFIG_LIBRARIES})
