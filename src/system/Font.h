#pragma once

#include "Color.h"

#include <memory>
#include <string>

class Texture;


enum class TextAlign : uint8_t { LEFT, RIGHT };

class Font {
public:
    virtual ~Font() {}
    virtual std::unique_ptr<Texture> renderText(const std::string&, const RGBColor&,
                                                TextAlign align = TextAlign::LEFT) = 0;
    virtual std::unique_ptr<Texture> renderText(const std::string&, const RGBAColor&,
                                                TextAlign align = TextAlign::LEFT) = 0;
};
