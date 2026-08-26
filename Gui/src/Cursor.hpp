#pragma once

#include <Point.hpp>
#include <Types.hpp>

namespace gui
{
    
    enum class CursorType
    {
        Arrow,
        Hand,
    };

    class Cursor
    {
    public:
       
        static Point<int> getPosition() noexcept;
        static void setPosition(int32 x, int32 y) noexcept;

        static void setType(CursorType localType) noexcept;
        static CursorType getType() noexcept;

      private:
        inline static CursorType type = CursorType::Arrow;
    };
} // namespace gui