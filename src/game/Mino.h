#pragma once

#include <stdint.h>


class GraphicsContext;
enum class TexID : uint8_t;


/// A Mino represents one block of a piece.
class Mino {
public:
    /// Create a mino with the texture and Ascii value
    Mino(TexID texture, char ascii_val);

    /// Draw the Mino at the provided coordinates
    void draw(GraphicsContext&, int x, int y);
    /// The Ascii value of the Mino, used mainly for debugging
    char asAscii() const { return ascii_val; }

private:
    const TexID texture;
    const char ascii_val;

    static const int8_t texture_size_px;
};
