#include "UnitTest++/UnitTest++.h"

#include "TestUtils.h"
#include "system/GraphicsContext.h"


const std::string SCREENSHOT_NAME = std::tmpnam(nullptr);

class AppContext {
public:
    AppContext()
        : gcx(GraphicsContext::create())
    {
        // TODO: set window and resolution to 640x480
    }

protected:
    std::unique_ptr<GraphicsContext> gcx;
};


enum class FontID: uint8_t {
    REGULAR,
};

enum class TexID: uint8_t {
    GREENRECT,

    // Test 1
    T1_ASCII,
    T1_LATIN1,
    T1_LATIN2,
    T1_CYRILLIC,
    T1_JAPANESE,

    // Test 2
    T2_LINEBREAK,
};


SUITE(GraphicsContext) {

TEST_FIXTURE(AppContext, Create) {
    // do nothing
}

TEST_FIXTURE(AppContext, SaveScreenshot) {
    gcx->requestScreenshot(SCREENSHOT_NAME);
    gcx->render();
}

TEST_FIXTURE(AppContext, TextRendering) {
    gcx->loadFont(FontID::REGULAR, "data/regular.otf", 20);

    // The regular font must support all english characters
    gcx->cacheText(TexID::T1_ASCII, "The quick brown fox jumped over the lazy dog",
                   FontID::REGULAR, 0xFFFFFF_rgb);
    // Some games have german or greek letter (eg. über, alpha)
    gcx->cacheText(TexID::T1_LATIN1, "Zwölf Boxkämpfer über den großen αβγδεφχψω ΔΣΦΨΩ",
                   FontID::REGULAR, 0xFFFFFF_rgb);
    // TODO: for future localization, add Latin2 support
    gcx->cacheText(TexID::T1_LATIN2, "Árvíztűrő tükörfúrógép Pchnąć w tę łódź",
                   FontID::REGULAR, 0xFFFFFF_rgb);
    // TODO: for future localization, add non-latin language support
    gcx->cacheText(TexID::T1_CYRILLIC, "Широкая электрификация южных",
                   FontID::REGULAR, 0xFFFFFF_rgb);
    // The regular font must support hiragana, katakana and the most common kanjis
    gcx->cacheText(TexID::T1_JAPANESE, "色は匂へど散りぬるを イロハニホヘト 鳥啼く声す 夢覚ませ",
                   FontID::REGULAR, 0xFFFFFF_rgb);

    gcx->drawTexture(TexID::T1_ASCII, 10, 10);
    gcx->drawTexture(TexID::T1_LATIN1, 10, 50);
    // gcx->drawTexture(TexID::T1_LATIN2, 10, 90);
    // gcx->drawTexture(TexID::T1_CYRILLIC, 10, 130);
    gcx->drawTexture(TexID::T1_JAPANESE, 10, 170);
    gcx->requestScreenshot(SCREENSHOT_NAME);
    gcx->render();

    CHECK(TestUtils::imageCompare("tests/references/text_multilang.png", SCREENSHOT_NAME));
}

TEST_FIXTURE(AppContext, TextLinebreak) {
    gcx->loadFont(FontID::REGULAR, "data/regular.otf", 30);
    gcx->cacheText(TexID::T2_LINEBREAK, "There should be\nthree lines\non the screen",
                   FontID::REGULAR, 0xFFFFFF_rgb);

    gcx->drawTexture(TexID::T2_LINEBREAK, 10, 10);
    gcx->requestScreenshot(SCREENSHOT_NAME);
    gcx->render();

    CHECK(TestUtils::imageCompare("tests/references/text_multiline.png", SCREENSHOT_NAME));
}

TEST_FIXTURE(AppContext, TextEmpty) {
    gcx->loadFont(FontID::REGULAR, "data/regular.otf", 30);

    CHECK_THROW(
        gcx->cacheText(TexID::T2_LINEBREAK, "",
                       FontID::REGULAR, 0xFFFFFF_rgb),
        std::runtime_error
    );
}

TEST_FIXTURE(AppContext, DrawRect) {
    gcx->drawFilledRect({10, 20, 120, 100}, 0xFF0000_rgb);
    gcx->drawFilledRect({50, 60, 120, 100}, 0x00FF00_rgb);
    gcx->drawFilledRect({100, 110, 120, 100}, 0x0000FF_rgb);
    gcx->requestScreenshot(SCREENSHOT_NAME);
    gcx->render();

    CHECK(TestUtils::imageCompare("tests/references/draw_filledrect.png", SCREENSHOT_NAME));
}

TEST_FIXTURE(AppContext, LoadImage) {
    gcx->loadTexture(TexID::GREENRECT, "tests/data/green_rect.png");
    gcx->drawTexture(TexID::GREENRECT, 10, 10);
    gcx->requestScreenshot(SCREENSHOT_NAME);
    gcx->render();

    CHECK(TestUtils::imageCompare("tests/references/draw_image.png", SCREENSHOT_NAME));
}

TEST_FIXTURE(AppContext, DrawScaled) {
    gcx->loadTexture(TexID::GREENRECT, "tests/data/green_rect.png");
    gcx->drawTexture(TexID::GREENRECT, {10, 20, 50, 50});
    gcx->requestScreenshot(SCREENSHOT_NAME);
    gcx->render();

    CHECK(TestUtils::imageCompare("tests/references/draw_scaled.png", SCREENSHOT_NAME));
}

} // SUITE
