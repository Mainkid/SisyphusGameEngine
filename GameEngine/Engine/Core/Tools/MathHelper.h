#pragma once
struct SyMathHelper
{
    inline static constexpr float EPS = 0.001f;
    inline static constexpr float PI = 3.141592f;

    static bool AreEqual(float f1, float f2, float eps = EPS)
    {
        return abs(f1 - f2) < eps;
    }

    static bool AreEqual(const SyVector3& v1, const SyVector3& v2)
    {
        return AreEqual(v1.x, v2.x) && AreEqual(v1.y, v2.y) && AreEqual(v1.z, v2.z);
    }
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

    template <typename T>
    static T Sign(const T& value)
    {
        if (value > 0)
            return (T)1;
        if (value == 0)
            return (T)0;
        return (T)-1;
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

    //Get a bit value of an integer number
    static int Bit(int number, int bitNum)
    {
        return (number & (1 << bitNum)) >> bitNum;
    }
};