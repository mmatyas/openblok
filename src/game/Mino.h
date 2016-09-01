#pragma once

#include <stdint.h>


class GraphicsContext;
enum class TexID : uint8_t;


class Mino {
public:
    Mino(TexID, char ascii_val);

    void draw(GraphicsContext&, int x, int y);
    char asAscii() const { return ascii_val; }

    enum class Type {
        I, J, L, O, S, T, Z
    };

private:
    const TexID texture;
    const char ascii_val;

    static const int8_t texture_size_px;
};
