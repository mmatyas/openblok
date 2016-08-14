set(MOD_SYSTEM_SRC
	GraphicsContext.cpp

	# SDL2
	sdl/SDLGraphicsContext.cpp
)

set(MOD_SYSTEM_H
	GraphicsContext.h
)

add_library(module_system ${MOD_SYSTEM_SRC} ${MOD_SYSTEM_H})

# use SDL2
include_directories(${SDL2PP_INCLUDE_DIRS})
target_link_libraries(module_system ${SDL2PP_LIBRARIES})
