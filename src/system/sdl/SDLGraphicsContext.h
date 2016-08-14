#pragma once

#include "../GraphicsContext.h"

class SDLGraphicsContext: public GraphicsContext {
public:
	virtual void toggleFullscreen();
	virtual uint16_t screenWidth();
	virtual uint16_t screenHeight();
};
