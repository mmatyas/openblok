#pragma once

#include "system/ResourceTypes.h"


class GraphicsContext;

namespace GameplayResources {

struct Textures {
    static TextureID MINO_I;
    static TextureID MINO_J;
    static TextureID MINO_L;
    static TextureID MINO_O;
    static TextureID MINO_S;
    static TextureID MINO_T;
    static TextureID MINO_Z;
    static TextureID MINO_GHOST;
    static TextureID MATRIXBG;

    static void loadAll(GraphicsContext&);
    static void unloadAll(GraphicsContext&);
};

} // namespace GameplayResources
