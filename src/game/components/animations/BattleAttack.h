#pragma once

#include "game/Transition.h"

#include <memory>

class Font;
class Mino;
class Texture;


class BattleAttackAnim {
public:
    BattleAttackAnim(int start_x, int width, int center_y, int arc_y, std::function<void()>&& callback = []{});

    void update();
    void draw() const;

    bool isActive() const { return arc_percent.running(); }

private:
    int arc_center_x, arc_center_y;
    int arc_radius;
    double arc_angle_start, arc_angle_end;
    double arc_angle_diff;
    double arc_x, arc_y;
    Transition<double> arc_percent;

    std::shared_ptr<Mino> mino;
};
