#include "Cursor.hpp"

#if defined(_WIN32)
#include <Windows.h>
#endif

namespace gui
{
    Point<int> Cursor::getPosition() noexcept
    {
        Point<int> pos = {0, 0};

#if defined(_WIN32)
        ::POINT win32Pt;
        if (::GetCursorPos(&win32Pt))
        {
            pos.x = static_cast<int32>(win32Pt.x);
            pos.y = static_cast<int32>(win32Pt.y);
        }
#endif

        return pos;
    }

    void Cursor::setPosition(int32 x, int32 y) noexcept
    {
#if defined(_WIN32)
        ::SetCursorPos(static_cast<int>(x), static_cast<int>(y));
#endif
    }
};
