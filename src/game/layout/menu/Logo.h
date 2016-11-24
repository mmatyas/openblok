#pragma once

#include "game/layout/Box.h"

#include <memory>


class GraphicsContext;
class Texture;


namespace Layout {
class Logo : public Box {
public:
    Logo(GraphicsContext&, int height_px);
    void draw() const;

private:
    std::unique_ptr<Texture> tex;
};
} // namespace Layout

