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
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(module_system ${SDL2_LIBRARY})
