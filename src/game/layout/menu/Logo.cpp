#include "Logo.h"

#include "game/AppContext.h"
#include "system/GraphicsContext.h"
#include "system/Texture.h"


namespace Layout {

Logo::Logo(GraphicsContext& gcx, int height_px)
    : tex(gcx.loadTexture(DATADIR + "logo.png"))
{

    bounding_box.h = height_px;
    float ratio = height_px * 1.f / tex->height();
    bounding_box.w = tex->width() * ratio;
}

void Logo::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;
}

void Logo::draw() const
{
    tex->drawScaled(bounding_box);
}

} // namespace Layout
