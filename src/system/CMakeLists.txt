set(MOD_SYSTEM_SRC
	Log.cpp
	EventCollector.cpp
	InputEvent.cpp
	GraphicsContext.cpp

	# SDL2
	sdl/SDLEventCollector.cpp
	sdl/SDLGraphicsContext.cpp
)

set(MOD_SYSTEM_H
	Log.h
	EventCollector.h
	InputEvent.h
	GraphicsContext.h
)

add_library(module_system ${MOD_SYSTEM_SRC} ${MOD_SYSTEM_H})

# use SDL2
include_directories(${SDL2PP_INCLUDE_DIRS})
target_link_libraries(module_system ${SDL2PP_LIBRARIES})
