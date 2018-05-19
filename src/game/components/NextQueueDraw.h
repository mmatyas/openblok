#pragma once

#include "NextQueue.h"

class GraphicsContext;


/// Draw the N previewable pieces at (x,y)
void drawNextQueue(const NextQueue&, GraphicsContext&, int x, int y);
