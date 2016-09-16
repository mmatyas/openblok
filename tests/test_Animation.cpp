#include "UnitTest++/UnitTest++.h"

#include "game/Animation.h"

#include <cmath>


SUITE(Animation) {
    using namespace std::chrono_literals;

TEST(CreateVoid) {
    bool val = false;

    Animation<void> anim(1s, [&val](double){ val = true; });
    CHECK_EQUAL(true, anim.running());
    CHECK_EQUAL(true, val);

    val = false;
    anim.update(1s);
    CHECK_EQUAL(true, val);
    CHECK_EQUAL(false, anim.running());

    // never calls again
    val = false;
    anim.update(1s);
    CHECK_EQUAL(false, val);
    CHECK_EQUAL(false, anim.running());
}

TEST(CreateT) {
    bool val = false;

    Animation<int> anim(1s, [&val](double){ val = true; return 0; });
    CHECK_EQUAL(true, anim.running());
    CHECK_EQUAL(true, val);

    val = false;
    anim.update(1s);
    CHECK_EQUAL(true, val);
    CHECK_EQUAL(false, anim.running());

    // never calls again
    val = false;
    anim.update(1s);
    CHECK_EQUAL(false, val);
    CHECK_EQUAL(false, anim.running());
}

TEST(AnimatorRunsMultipleTimes) {
    int val = 0;

    Animation<void> anim_bind(3s, [&val](double t){ val = std::round(t * 3); });
    CHECK_EQUAL(0, val);
    anim_bind.update(1s);
    CHECK_EQUAL(1, val);
    anim_bind.update(1s);
    CHECK_EQUAL(2, val);
    anim_bind.update(1s);
    CHECK_EQUAL(3, val);

    Animation<int> anim_inner(3s, [](double t){ return std::round(t * 3); });
    CHECK_EQUAL(0, anim_inner.value());
    anim_inner.update(1s);
    CHECK_EQUAL(1, anim_inner.value());
    anim_inner.update(1s);
    CHECK_EQUAL(2, anim_inner.value());
    anim_inner.update(1s);
    CHECK_EQUAL(3, anim_inner.value());
}

TEST(OnEndCallbackIsCalled) {
    int val = 0;
    bool finished = false;

    Animation<void> anim_bind(3s, [&val](double t){ val = std::round(t * 3); }, [&finished](){ finished = true; });
    CHECK_EQUAL(0, val);
    CHECK_EQUAL(false, finished);
    anim_bind.update(1s);
    CHECK_EQUAL(1, val);
    CHECK_EQUAL(false, finished);
    anim_bind.update(1s);
    CHECK_EQUAL(2, val);
    CHECK_EQUAL(false, finished);
    anim_bind.update(1s);
    CHECK_EQUAL(3, val);
    CHECK_EQUAL(true, finished);

    // never call again
    anim_bind.update(1s);
    CHECK_EQUAL(3, val);
    CHECK_EQUAL(true, finished);
}

TEST(StopReset) {
    int val = 50;
    Animation<void> anim(3s,
        [&val](double t){ val = std::round(t * 3); },
        [&val](){ val += 10; });

    CHECK_EQUAL(0, val);
    CHECK_EQUAL(true, anim.running());

    val = 50;
    anim.stop();
    CHECK_EQUAL(50, val);
    CHECK_EQUAL(false, anim.running());

    anim.reset();
    CHECK_EQUAL(0, val);
    CHECK_EQUAL(true, anim.running());
}

} // Suite
