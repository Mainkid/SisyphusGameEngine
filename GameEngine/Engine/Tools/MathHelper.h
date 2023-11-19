#pragma once

struct SyMathHelper
{
    inline static constexpr float PI = 3.141592f;
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

    template <typename T>
    static const T& Clamp(const T& val, const T& min, const T& max)
    {
        if (val < min)
            return min;
        if (val > max)
            return max;
        return val;
    }

    static float CalculateSphereVolume(float radius)
    {
        return 4.0f * PI * radius * radius * radius / 3;
    }
    static float CalculateBoxVolume(const SyVector3& extent)
    {
        return 8 * extent.x * extent.y * extent.z;
    }
    static float CalculateCapsuleVolume(float radius, float halfHeight)
    {
        return CalculateSphereVolume(radius) + PI * radius * radius * 2 * halfHeight;
    }
};