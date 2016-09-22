set(MOD_SYSTEM_SRC
	Color.cpp
	EventCollector.cpp
	FontManager.cpp
	GraphicsContext.cpp
	InputEvent.cpp
	Log.cpp

	# SDL2
	sdl/SDLEventCollector.cpp
	sdl/SDLGraphicsContext.cpp

	# FontConfig
	fontconfig/FontConfig.cpp
)

set(MOD_SYSTEM_H
	Color.h
	EventCollector.h
	FontManager.h
	GraphicsContext.h
	InputEvent.h
	Localize.h
	Log.h

	# SDL2
	sdl/SDLEventCollector.h
	sdl/SDLGraphicsContext.h

	# FontConfig
	fontconfig/FontConfig.h
)

add_library(module_system ${MOD_SYSTEM_SRC} ${MOD_SYSTEM_H})

# use SDL2
include_directories(${SDL2PP_INCLUDE_DIRS})
target_link_libraries(module_system ${SDL2PP_LIBRARIES} ${FONTCONFIG_LIBRARIES})
