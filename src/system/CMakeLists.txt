set(MOD_SYSTEM_SRC
	EventCollector.cpp
	GraphicsContext.cpp
	InputEvent.cpp
	Log.cpp

	# SDL2
	sdl/SDLEventCollector.cpp
	sdl/SDLGraphicsContext.cpp
)

set(MOD_SYSTEM_H
	EventCollector.h
	GraphicsContext.h
	InputEvent.h
	Log.h

	# SDL2
	sdl/SDLEventCollector.h
	sdl/SDLGraphicsContext.h
)

add_library(module_system ${MOD_SYSTEM_SRC} ${MOD_SYSTEM_H})

# use SDL2
include_directories(${SDL2PP_INCLUDE_DIRS})
target_link_libraries(module_system ${SDL2PP_LIBRARIES})
