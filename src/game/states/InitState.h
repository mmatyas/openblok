#pragma once

#include "game/GameState.h"
#include "system/ResourceTypes.h"

#include <future>


class InitState: public GameState {
public:
    InitState(AppContext&);
    void update(const std::vector<InputEvent>&, AppContext&) final;
    void draw(GraphicsContext& gcx) final;

private:
    std::future<void> worker;
    TextureID tex_loading;
};
