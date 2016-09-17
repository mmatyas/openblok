#pragma once

#include <chrono>
#include <functional>
#include <assert.h>


class AnimationBase {
public:
    using Duration = std::chrono::steady_clock::duration;

    virtual ~AnimationBase() {};

    /// Returns true if the animation did not finish yet.
    virtual bool running() const { return is_running; }

    /// Stops the animation.
    virtual void stop() {
        is_running = false;
    }

protected:
    AnimationBase(Duration duration, std::function<void()> on_end)
        : duration(duration)
        , timer(Duration::zero())
        , end_callback(on_end)
        , is_running(true)
    {
        assert(duration > Duration::zero());
    }

    void reset() {
        is_running = true;
        timer = Duration::zero();
    }

    Duration duration;
    Duration timer;
    std::function<void()> end_callback;
    bool is_running;
};

/// An Animation is responsible for repeatedly calling a specified function
/// for a duration of time, with the percent of already passed time as parameter.
template<typename T>
class Animation : public AnimationBase {
public:
    /// Crates a new animation witha specified duration, animation function,
    /// and an optional callback which would be invoked on completion.
    ///
    /// The animation function receives the elapsed time as a value in the range 0.0-1.0.
    /// If it has a return value, the result will be stored, and can be later queried by
    /// the Animation's value() member.
    ///
    /// When creating the Animation, on_update will be called with the value 0.0 .
    /// After the elapsed time reaches the duration (by repeatedly calling update()), on_update will
    /// be called with the value 1.0, then on_end_cb will be called,
    /// after that the animation stops and never calls any of the functions again.
    Animation(Duration duration, std::function<T(double)> on_update, std::function<void()> on_end = [](){})
        : AnimationBase(duration, on_end)
        , animator(on_update)
        , last_value(animator(0.0))
    {}

    /// Replace the on-update function.
    void replaceFn(std::function<T(double)> on_update) {
        animator = on_update;
        update(Duration::zero());
    }

    /// Update the animation's timer with the specified amount of time.
    /// The animator function will be called.
    void update(Duration elapsed) {
        if (!is_running)
            return;

        timer += elapsed;
        if (timer >= duration) {
            is_running = false;
            last_value = animator(1.0);
            end_callback();
        }
        else
            last_value = animator(timer / (duration * 1.0));
    }

    /// Restart the animation.
    void reset() {
        AnimationBase::reset();
        last_value = animator(0.0);
    }

    /// Returns the last result of the animation function
    const T& value() const { return last_value; }

private:
    std::function<T(double)> animator;
    T last_value;
};

template<>
class Animation<void> : public AnimationBase {
    using Duration = std::chrono::steady_clock::duration;

public:
    Animation(Duration duration, std::function<void(double)> on_update, std::function<void()> on_end = [](){})
        : AnimationBase(duration, on_end)
        , animator(on_update)
    {
        on_update(0.0);
    }

    void replaceFn(std::function<void(double)> on_update) {
        animator = on_update;
        update(Duration::zero());
    }

    void update(Duration elapsed) {
        if (!is_running)
            return;

        timer += elapsed;
        if (timer >= duration) {
            is_running = false;
            animator(1.0);
            end_callback();
        }
        else
            animator(timer / (duration * 1.0));
    }

    void reset() {
        AnimationBase::reset();
        animator(0.0);
    }

    void value() const {};

private:
    std::function<void(double)> animator;
};
