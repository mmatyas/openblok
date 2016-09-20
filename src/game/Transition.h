#pragma once

#include <chrono>
#include <functional>
#include <assert.h>


class TransitionBase {
public:
    using Duration = std::chrono::steady_clock::duration;

    virtual ~TransitionBase() {};

    /// Returns true if the animation did not finish yet.
    virtual bool running() const { return is_running; }

    /// Stops the animation, but it can be continued from this point later.
    virtual void pause() {
        is_running = false;
    }

    /// Continues the animation from where it was left off.
    virtual void unpause() {
        is_running = true;
    }

    /// Stops the animation and resets its timer to zero.
    virtual void stop() {
        is_running = false;
        timer = Duration::zero();
    }

protected:
    TransitionBase(Duration duration, std::function<void()> on_end)
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

/// A Transition is responsible for repeatedly calling a specified function
/// for a duration of time, with the percent of already passed time as parameter.
template<typename T>
class Transition : public TransitionBase {
public:
    /// Crates a new transition with a specified duration, animation function,
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
    Transition(Duration duration, std::function<T(double)> on_update, std::function<void()> on_end = [](){})
        : TransitionBase(duration, on_end)
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
        TransitionBase::reset();
        last_value = animator(0.0);
    }

    /// Returns the last result of the animation function
    const T& value() const { return last_value; }

private:
    std::function<T(double)> animator;
    T last_value;
};

template<>
class Transition<void> : public TransitionBase {
    using Duration = std::chrono::steady_clock::duration;

public:
     Transition(Duration duration, std::function<void(double)> on_update, std::function<void()> on_end = [](){})
        : TransitionBase(duration, on_end)
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
        TransitionBase::reset();
        animator(0.0);
    }

    void value() const {};

private:
    std::function<void(double)> animator;
};
