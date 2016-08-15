set(MOD_SYSTEM_SRC
	EventCollector.cpp
	InputEvent.cpp
	GraphicsContext.cpp
	Log.cpp

	# SDL2
	sdl/SDLEventCollector.cpp
	sdl/SDLGraphicsContext.cpp
)

set(MOD_SYSTEM_H
	EventCollector.h
	InputEvent.h
	GraphicsContext.h
	Log.h
)

add_library(module_system ${MOD_SYSTEM_SRC} ${MOD_SYSTEM_H})

# use SDL2
include_directories(${SDL2PP_INCLUDE_DIRS})
target_link_libraries(module_system ${SDL2PP_LIBRARIES})
