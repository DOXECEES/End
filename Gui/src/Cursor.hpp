#pragma once

#include <Point.hpp>
#include <Types.hpp>

namespace gui
{
    
    class Cursor
    {
    public:
       
        static Point<int> getPosition() noexcept;

        static void setPosition(int32 x, int32 y) noexcept;

    };
} // namespace gui