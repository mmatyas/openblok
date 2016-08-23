#include "UnitTest++/UnitTest++.h"

#include "system/GraphicsContext.h"


class AppContext {
public:
    AppContext()
        : gcx(GraphicsContext::create())
    {}

protected:
    std::unique_ptr<GraphicsContext> gcx;
};


enum class ResourceID: uint8_t {
    TEST_FONT,
    TEST_TEX_ASCII,
    TEST_TEX_LATIN2,
    TEST_TEX_CYRILLIC,
    TEST_TEX_JAPANESE,
};


TEST_FIXTURE(AppContext, Create) {
    // do nothing
}

TEST_FIXTURE(AppContext, SaveScreenshot) {
    gcx->requestScreenshot("test_screen.bmp");
    gcx->render();
}

TEST_FIXTURE(AppContext, TextRendering) {
    gcx->loadFont(ResourceID::TEST_FONT, "data/regular.otf", 30);
    gcx->cacheText(ResourceID::TEST_TEX_ASCII, "The quick brown fox jumped over the lazy dog",
                   ResourceID::TEST_FONT, {0xFF, 0xFF, 0xFF, 0xFF});
    gcx->cacheText(ResourceID::TEST_TEX_LATIN2, "Árvíztűrő tükörfúrógép",
                   ResourceID::TEST_FONT, {0xFF, 0xFF, 0xFF, 0xFF});
    gcx->cacheText(ResourceID::TEST_TEX_CYRILLIC, "Широкая электрификация южных",
                   ResourceID::TEST_FONT, {0xFF, 0xFF, 0xFF, 0xFF});
    gcx->cacheText(ResourceID::TEST_TEX_JAPANESE, "色は匂へど散りぬるを",
                   ResourceID::TEST_FONT, {0xFF, 0xFF, 0xFF, 0xFF});

    // TODO
    gcx->drawTexture(ResourceID::TEST_TEX_ASCII, 10, 10);
    gcx->drawTexture(ResourceID::TEST_TEX_LATIN2, 10, 50);
    gcx->drawTexture(ResourceID::TEST_TEX_CYRILLIC, 10, 90);
    // gcx->drawTexture(ResourceID::TEST_TEX_JAPANESE, 10, 130);
    gcx->requestScreenshot("test_screen.bmp");
    gcx->render();
}
