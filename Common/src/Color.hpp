#pragma once

#include "Types.hpp"

struct Color
{
    static constexpr Color fromRgba(uint8 r, uint8 g, uint8 b, uint8 a)
    {
        return Color { r, g, b, a };
    }

    static constexpr Color fromRgb(uint8 r, uint8 g, uint8 b)
    {
        return Color { r, g, b, 255 };
    }

    uint8 r;
    uint8 g;
    uint8 b;
    uint8 a;
};

namespace Colors
{
    inline constexpr Color White       = { 255, 255, 255, 255 };
    inline constexpr Color Black       = { 0, 0, 0, 255 };
    inline constexpr Color Transparent = { 0, 0, 0, 0 };

    inline constexpr Color Red   = { 255, 0, 0, 255 };
    inline constexpr Color Green = { 0, 255, 0, 255 };
    inline constexpr Color Blue  = { 0, 0, 255, 255 };

    inline constexpr Color Yellow  = { 255, 255, 0, 255 };
    inline constexpr Color Cyan    = { 0, 255, 255, 255 };
    inline constexpr Color Magenta = { 255, 0, 255, 255 };

    inline constexpr Color Orange    = { 255, 165, 0, 255 };
    inline constexpr Color Purple    = { 128, 0, 128, 255 };
    inline constexpr Color Gray      = { 128, 128, 128, 255 };
    inline constexpr Color LightGray = { 192, 192, 192, 255 };
    inline constexpr Color DarkGray  = { 64, 64, 64, 255 };
} // namespace Colors
