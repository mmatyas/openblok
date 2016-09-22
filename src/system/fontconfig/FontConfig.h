#pragma once

#include "system/FontManager.h"

#include <fontconfig/fontconfig.h>


class FontConfig : public FontManager {
public:
    FontConfig();

    std::string find(const std::string& family, const std::string& style = "") final;
    std::vector<std::string> findAll(const std::string& family, const std::string& style = "") final;

private:
    std::unique_ptr<FcConfig, std::function<void(FcConfig*)>> config;

    std::unique_ptr<FcPattern, std::function<void(FcPattern*)>>
    createPattern(const std::string& family, const std::string& style);
};
