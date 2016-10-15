#pragma once

#include "Color.h"

#include <memory>
#include <string>


class Texture;

class Font {
public:
    virtual ~Font() {}
    virtual std::unique_ptr<Texture> renderText(const std::string&, const RGBColor&) = 0;
};
