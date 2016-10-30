#include "UnitTest++/UnitTest++.h"

#include "TestUtils.h"
#include "system/Font.h"
#include "system/Window.h"
#include "system/GraphicsContext.h"
#include "system/Texture.h"


class AppContext {
public:
    AppContext()
        : window(Window::create())
    {
        // TODO: set window and resolution to 640x480
    }

    GraphicsContext& gcx() {
        return window->graphicsContext();
    }

protected:
    std::unique_ptr<Window> window;
};


SUITE(GraphicsContext) {

TEST_FIXTURE(AppContext, Create) {
    // do nothing
}

TEST_FIXTURE(AppContext, SaveScreenshot) {
    window->requestScreenshot(std::tmpnam(nullptr));
    gcx().render();
}

TEST_FIXTURE(AppContext, TextRendering) {
    auto font = gcx().loadFont("data/fonts/PTC75F.ttf", 20);

    // The regular font must support all english characters
    auto ascii = font->renderText("The quick brown fox jumped over the lazy dog", 0xFFFFFF_rgb);
    // Some games have german or greek letter (eg. über, alpha)
    auto latin1 = font->renderText("Zwölf Boxkämpfer über den großen αβγδεφχψω ΔΣΦΨΩ", 0xFFFFFF_rgb);
    // TODO: for future localization, add Latin2 support
    auto latin2 = font->renderText("Árvíztűrő tükörfúrógép Pchnąć w tę łódź", 0xFFFFFF_rgb);
    // TODO: for future localization, add non-latin language support
    auto cyrillic = font->renderText("Широкая электрификация южных", 0xFFFFFF_rgb);
    // The regular font must support hiragana, katakana and the most common kanjis
    auto japanese = font->renderText("色は匂へど散りぬるを イロハニホヘト 鳥啼く声す 夢覚ませ", 0xFFFFFF_rgb);

    ascii->drawAt(10, 10);
    latin1->drawAt(10, 50);
    latin2->drawAt(10, 90);
    cyrillic->drawAt(10, 130);
    japanese->drawAt(10, 170);

    const std::string screenshot_path = std::tmpnam(nullptr);
    window->requestScreenshot(screenshot_path);
    gcx().render();

    CHECK(TestUtils::imageCompare("tests/references/text_multilang.png", screenshot_path));
}

TEST_FIXTURE(AppContext, TextLinebreak) {
    auto font = gcx().loadFont("data/fonts/PTC75F.ttf", 30);
    auto tex = font->renderText("There should be\nthree lines\non the screen", 0xFFFFFF_rgb);

    tex->drawAt(10, 10);

    const std::string screenshot_path = std::tmpnam(nullptr);
    window->requestScreenshot(screenshot_path);
    gcx().render();

    CHECK(TestUtils::imageCompare("tests/references/text_multiline.png", screenshot_path));
}

TEST_FIXTURE(AppContext, TextEmpty) {
    auto font = gcx().loadFont("data/fonts/PTC75F.ttf", 30);

    CHECK_THROW(
        font->renderText("", 0xFFFFFF_rgb),
        std::runtime_error
    );
}

TEST_FIXTURE(AppContext, DrawRect) {
    gcx().drawFilledRect({10, 20, 120, 100}, 0xFF0000_rgb);
    gcx().drawFilledRect({50, 60, 120, 100}, 0x00FF00_rgb);
    gcx().drawFilledRect({100, 110, 120, 100}, 0x0000FF_rgb);

    const std::string screenshot_path = std::tmpnam(nullptr);
    window->requestScreenshot(screenshot_path);
    gcx().render();

    CHECK(TestUtils::imageCompare("tests/references/draw_filledrect.png", screenshot_path));
}

TEST_FIXTURE(AppContext, LoadImage) {
    auto tex = gcx().loadTexture("tests/data/green_rect.png");
    tex->drawAt(10, 10);

    const std::string screenshot_path = std::tmpnam(nullptr);
    window->requestScreenshot(screenshot_path);
    gcx().render();


    CHECK(TestUtils::imageCompare("tests/references/draw_image.png", screenshot_path));
}

TEST_FIXTURE(AppContext, DrawScaled) {
    auto tex = gcx().loadTexture("tests/data/green_rect.png");
    tex->drawScaled({10, 20, 50, 50});

    const std::string screenshot_path = std::tmpnam(nullptr);
    window->requestScreenshot(screenshot_path);
    gcx().render();

    CHECK(TestUtils::imageCompare("tests/references/draw_scaled.png", screenshot_path));
}

} // SUITE
