#include "UnitTest++/UnitTest++.h"

#include "TestUtils.h"
#include "system/GraphicsContext.h"


const std::string SCREENSHOT_NAME = "test.bmp";

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
    TEST_TEX_LATIN1,
    TEST_TEX_LATIN2,
    TEST_TEX_CYRILLIC,
    TEST_TEX_JAPANESE,
};


TEST_FIXTURE(AppContext, Create) {
    // do nothing
}

TEST_FIXTURE(AppContext, SaveScreenshot) {
    gcx->requestScreenshot(SCREENSHOT_NAME);
    gcx->render();
}

TEST_FIXTURE(AppContext, TextRendering) {
    gcx->loadFont(ResourceID::TEST_FONT, "data/regular.otf", 20);

    // The regular font must support all english characters
    gcx->cacheText(ResourceID::TEST_TEX_ASCII, "The quick brown fox jumped over the lazy dog",
                   ResourceID::TEST_FONT, {0xFF, 0xFF, 0xFF, 0xFF});
    // Some games have german or greek letter (eg. über, alpha)
    gcx->cacheText(ResourceID::TEST_TEX_LATIN1, "Zwölf Boxkämpfer über den großen αβγδεφχψω ΔΣΦΨΩ",
                   ResourceID::TEST_FONT, {0xFF, 0xFF, 0xFF, 0xFF});
    // TODO: for future localization, add Latin2 support
    gcx->cacheText(ResourceID::TEST_TEX_LATIN2, "Árvíztűrő tükörfúrógép Pchnąć w tę łódź",
                   ResourceID::TEST_FONT, {0xFF, 0xFF, 0xFF, 0xFF});
    // TODO: for future localization, add non-latin language support
    gcx->cacheText(ResourceID::TEST_TEX_CYRILLIC, "Широкая электрификация южных",
                   ResourceID::TEST_FONT, {0xFF, 0xFF, 0xFF, 0xFF});
    // The regular font must support hiragana, katakana and the most common kanjis
    gcx->cacheText(ResourceID::TEST_TEX_JAPANESE, "色は匂へど散りぬるを イロハニホヘト 鳥啼く声す 夢覚ませ",
                   ResourceID::TEST_FONT, {0xFF, 0xFF, 0xFF, 0xFF});

    gcx->drawTexture(ResourceID::TEST_TEX_ASCII, 10, 10);
    gcx->drawTexture(ResourceID::TEST_TEX_LATIN1, 10, 50);
    // gcx->drawTexture(ResourceID::TEST_TEX_LATIN2, 10, 90);
    // gcx->drawTexture(ResourceID::TEST_TEX_CYRILLIC, 10, 130);
    gcx->drawTexture(ResourceID::TEST_TEX_JAPANESE, 10, 170);
    gcx->requestScreenshot(SCREENSHOT_NAME);
    gcx->render();

    CHECK(TestUtils::imageCompare("tests/references/regular_text.png", SCREENSHOT_NAME));
}
