#include "InitState.h"

#include "game/Resources.h"
#include "game/AppContext.h"
#include "system/Log.h"


InitState::InitState(AppContext& app)
{
    app.gcx->loadFont(ResourceID::FONT_REGULAR, "data/regular.otf", 30);
    app.gcx->cacheText(ResourceID::TEX_LOADING, "LOADING...",
                       ResourceID::FONT_REGULAR, {0xFF, 0xFF, 0xFF, 0xFF});
}

void InitState::update(const std::vector<InputEvent>&, AppContext&)
{

}

void InitState::draw(GraphicsContext& gcx)
{
    gcx.drawTexture(ResourceID::TEX_LOADING);
}
