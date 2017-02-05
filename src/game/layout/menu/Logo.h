#pragma once

#include "game/layout/Box.h"

#include <memory>


class AppContext;
class Texture;


namespace Layout {
class Logo : public Box {
public:
    Logo(AppContext&, int height_px);
    void draw() const;

private:
    std::unique_ptr<Texture> tex;
};
} // namespace Layout

