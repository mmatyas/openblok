#pragma once

#include <stdint.h>


class GraphicsContext;
enum class TexID : uint8_t;


class Mino {
public:
    enum class Type {
        I, J, L, O, S, T, Z
    };

    Mino(Type, TexID);

    void draw(GraphicsContext&, int x, int y);
    char asAscii() const { return ascii_val; }

private:
    const Type type;
    const TexID texture;
    const char ascii_val;

    static const int8_t texture_size_px;
};
