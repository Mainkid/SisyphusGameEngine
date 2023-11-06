#pragma once

struct SyMathHelper
{
    template <typename T>
    static const T& Max(const T& x_, const T& y_)
    {
        if (x_ > y_)
            return x_;
        return y_;
    }

    template <typename T>
    static const T& Min(const T& x_, const T& y_)
    {
        if (x_ < y_)
            return x_;
        return y_;
    }
};