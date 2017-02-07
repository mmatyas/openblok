#include "Logo.h"

#include "game/AppContext.h"
#include "system/GraphicsContext.h"
#include "system/Paths.h"
#include "system/Texture.h"


namespace Layout {

Logo::Logo(AppContext& app, int height_px)
    : tex(app.gcx().loadTexture(app.theme().get_texture("logo.png")))
{
    bounding_box.h = height_px;
    float ratio = height_px * 1.f / tex->height();
    bounding_box.w = tex->width() * ratio;
}

void Logo::draw() const
{
    tex->drawScaled(bounding_box);
}

} // namespace Layout
