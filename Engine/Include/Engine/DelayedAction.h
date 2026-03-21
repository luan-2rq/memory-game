#pragma once

#include <functional>
#include <utility>

namespace Engine
{

class DelayedAction
{
public:
    using Callback = std::function<void()>;

    void start(float duration, Callback callback)
    {
        remaining_ = duration;
        callback_ = std::move(callback);
        active_ = true;
    }

    void cancel()
    {
        active_ = false;
        remaining_ = 0.f;
        callback_ = {};
    }

    void update(float dt, bool canAdvance = true)
    {
        if (!active_ || !canAdvance)
            return;

        remaining_ -= dt;
        if (remaining_ > 0.f)
            return;

        active_ = false;
        remaining_ = 0.f;

        Callback callback = std::move(callback_);
        callback_ = {};

        if (callback)
            callback();
    }

    [[nodiscard]] bool isActive() const
    {
        return active_;
    }

    [[nodiscard]] float remaining() const
    {
        return remaining_;
    }

private:
    float    remaining_{ 0.f };
    Callback callback_{};
    bool     active_{ false };
};

}