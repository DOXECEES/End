#pragma once
#include "Point.hpp"

template <typename T>
struct Rect
{
    T x      = T(0);
    T y      = T(0);
    T width  = T(0);
    T height = T(0);

    static constexpr Rect zero() noexcept
    {
        return { T(0), T(0), T(0), T(0) };
    }

    [[nodiscard]] bool contains(const Point<int>& pt) const noexcept
    {
        return (pt.x >= x && pt.x < x + width && pt.y >= y && pt.y < y + height);
    }
};