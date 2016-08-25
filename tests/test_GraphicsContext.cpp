#include "UnitTest++/UnitTest++.h"

#include "TestUtils.h"
#include "system/GraphicsContext.h"


const std::string SCREENSHOT_NAME = std::tmpnam(nullptr);

class AppContext {
public:
    AppContext()
        : gcx(GraphicsContext::create())
    {}

protected:
    std::unique_ptr<GraphicsContext> gcx;
};


enum class ResourceID: uint8_t {
    FONT_REGULAR,

    // Test 1
    T1_TEX_ASCII,
    T1_TEX_LATIN1,
    T1_TEX_LATIN2,
    T1_TEX_CYRILLIC,
    T1_TEX_JAPANESE,

    // Test 2
    T2_TEX_LINEBREAK
};


TEST_FIXTURE(AppContext, Create) {
    // do nothing
}

TEST_FIXTURE(AppContext, SaveScreenshot) {
    gcx->requestScreenshot(SCREENSHOT_NAME);
    gcx->render();
}

TEST_FIXTURE(AppContext, TextRendering) {
    gcx->loadFont(ResourceID::FONT_REGULAR, "data/regular.otf", 20);

    // The regular font must support all english characters
    gcx->cacheText(ResourceID::T1_TEX_ASCII, "The quick brown fox jumped over the lazy dog",
                   ResourceID::FONT_REGULAR, {0xFF, 0xFF, 0xFF, 0xFF});
    // Some games have german or greek letter (eg. über, alpha)
    gcx->cacheText(ResourceID::T1_TEX_LATIN1, "Zwölf Boxkämpfer über den großen αβγδεφχψω ΔΣΦΨΩ",
                   ResourceID::FONT_REGULAR, {0xFF, 0xFF, 0xFF, 0xFF});
    // TODO: for future localization, add Latin2 support
    gcx->cacheText(ResourceID::T1_TEX_LATIN2, "Árvíztűrő tükörfúrógép Pchnąć w tę łódź",
                   ResourceID::FONT_REGULAR, {0xFF, 0xFF, 0xFF, 0xFF});
    // TODO: for future localization, add non-latin language support
    gcx->cacheText(ResourceID::T1_TEX_CYRILLIC, "Широкая электрификация южных",
                   ResourceID::FONT_REGULAR, {0xFF, 0xFF, 0xFF, 0xFF});
    // The regular font must support hiragana, katakana and the most common kanjis
    gcx->cacheText(ResourceID::T1_TEX_JAPANESE, "色は匂へど散りぬるを イロハニホヘト 鳥啼く声す 夢覚ませ",
                   ResourceID::FONT_REGULAR, {0xFF, 0xFF, 0xFF, 0xFF});

    gcx->drawTexture(ResourceID::T1_TEX_ASCII, 10, 10);
    gcx->drawTexture(ResourceID::T1_TEX_LATIN1, 10, 50);
    // gcx->drawTexture(ResourceID::T1_TEX_LATIN2, 10, 90);
    // gcx->drawTexture(ResourceID::T1_TEX_CYRILLIC, 10, 130);
    gcx->drawTexture(ResourceID::T1_TEX_JAPANESE, 10, 170);
    gcx->requestScreenshot(SCREENSHOT_NAME);
    gcx->render();

    CHECK(TestUtils::imageCompare("tests/references/regular_text.png", SCREENSHOT_NAME));
}

TEST_FIXTURE(AppContext, TextLinebreak) {
    gcx->loadFont(ResourceID::FONT_REGULAR, "data/regular.otf", 30);
    gcx->cacheText(ResourceID::T2_TEX_LINEBREAK, "There should be\nthree lines\non the screen",
                   ResourceID::FONT_REGULAR, {0xFF, 0xFF, 0xFF, 0xFF});

    gcx->drawTexture(ResourceID::T2_TEX_LINEBREAK, 10, 10);
    gcx->requestScreenshot(SCREENSHOT_NAME);
    gcx->render();

    CHECK(TestUtils::imageCompare("tests/references/text_multiline.png", SCREENSHOT_NAME));
}
