#pragma once

#include "game/Transition.h"
#include "system/Color.h"

#include <memory>
#include <string>

class Font;
class Texture;


class TextPopup {
public:
    TextPopup(const std::string& text, std::shared_ptr<Font>& font);

    void update();
    void draw() const;

    unsigned width() const;
    float visibility() const { return alpha.value(); }

    bool isActive() const { return alpha.running(); }
    void setInitialPosition(int x, int y);

    static RGBColor text_color;

private:
    const std::string text;
    int pos_x, pos_y;
    Transition<int> pos_y_delta;
    Transition<uint8_t> alpha;

    std::unique_ptr<Texture> tex;
};
