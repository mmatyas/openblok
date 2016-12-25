#include "BattleAttack.h"

#include "game/components/Mino.h"
#include "game/components/MinoStorage.h"

#include <cmath>


BattleAttackAnim::BattleAttackAnim(int start_x, int width, int center_y, int arc_y, std::function<void()>&& callback)
    : arc_center_x(start_x + width / 2)
    , arc_center_y(center_y)
    , arc_percent(std::chrono::seconds(1), [](double t){ return t; }, std::move(callback))
    , mino(MinoStorage::getMino(PieceType::GARBAGE))
{
    assert(width != 0);
    assert(center_y < arc_y);

    const double dx1 = start_x - arc_center_x;
    const double dx2 = start_x + width - arc_center_x;
    const double dy = arc_y - center_y;
    arc_radius = std::sqrt(dx1 * dx1 + dy * dy);

    arc_angle_start = std::atan2(dy, dx1);
    arc_angle_end = std::atan2(dy, dx2);
    arc_angle_diff = arc_angle_end - arc_angle_start;

    arc_x = std::cos(arc_angle_start);
    arc_y = std::sin(arc_angle_start);
}

void BattleAttackAnim::update()
{
    arc_percent.update(Timing::frame_duration);

    const double angle = arc_angle_start + arc_angle_diff * arc_percent.value();

    arc_x = arc_center_x + std::cos(angle) * arc_radius;
    arc_y = arc_center_y + std::sin(angle) * arc_radius;
}

void BattleAttackAnim::draw() const
{
    static constexpr int offset = -Mino::texture_size_px / 2;
    mino->draw(arc_x + offset, arc_y + offset);
}
