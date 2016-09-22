#include "FontConfig.h"


void throwMatchError() {
    throw std::runtime_error("No matching font");
}

FontConfig::FontConfig()
{
    if (!FcInit())
        throw std::runtime_error("Could not initialize FontConfig");

    config = std::unique_ptr<FcConfig, std::function<void(FcConfig*)>>(
        FcInitLoadConfigAndFonts(),
        [](FcConfig* ptr){ FcConfigDestroy(ptr); });

    FcConfigAppFontAddDir(config.get(), (FcChar8*)"data/fonts");
}

std::unique_ptr<FcPattern, std::function<void(FcPattern*)>>
FontConfig::createPattern(const std::string& family, const std::string& style)
{
    auto pattern = std::unique_ptr<FcPattern, std::function<void(FcPattern*)>>(
        FcNameParse((FcChar8*)family.c_str()),
        [](FcPattern* ptr){ FcPatternDestroy(ptr); });

    if (style.length())
        FcPatternAddString(pattern.get(), "style", (FcChar8*)style.c_str());

    FcDefaultSubstitute(pattern.get());
    FcConfigSubstitute(config.get(), pattern.get(), FcMatchFont);

    return pattern;
}

std::string FontConfig::find(const std::string& family, const std::string& style)
{
    auto pattern = createPattern(family, style);

    FcResult query_result;
    auto query = std::unique_ptr<FcPattern, std::function<void(FcPattern*)>>(
        FcFontMatch(config.get(), pattern.get(), &query_result),
        [](FcPattern* ptr){ FcPatternDestroy(ptr); });

    if (query_result != FcResultMatch)
        throwMatchError();

    FcChar8* filename;
    if (FcPatternGetString(query.get(), FC_FILE, 0, &filename) != FcResultMatch)
        throwMatchError();

    return std::string((char*)filename);
}

std::vector<std::string> FontConfig::findAll(const std::string& family, const std::string& style)
{
    auto pattern = createPattern(family, style);

    FcResult query_result;
    auto query = std::unique_ptr<FcFontSet, std::function<void(FcFontSet*)>>(
        FcFontSort(config.get(), pattern.get(), false, nullptr, &query_result),
        [](FcFontSet* ptr){ FcFontSetDestroy(ptr); });

    if (query_result != FcResultMatch)
        throwMatchError();

    std::vector<std::string> output;
    for (int i = 0; i < query.get()->nfont; i++) {
        FcChar8* filename;
        if (FcPatternGetString(query.get()->fonts[i], FC_FILE, 0, &filename) != FcResultMatch)
            break;

        output.emplace_back((char*)filename);
    }

    if (!output.size())
        throwMatchError();

    return output;
}
