#include "WindowPositionerBatch.hpp"

#include <Windows.h>

#include "Window/Window.hpp"

namespace gui
{
    class WindowPositionerBatchImpl
    {
      public:
        WindowPositionerBatchImpl() noexcept = default;

        void beginBatch(uint8 count) noexcept
        {
            hdwp = ::BeginDeferWindowPos(count);
        }

        void endBatch() noexcept
        {
            ::EndDeferWindowPos(hdwp);
            hdwp = nullptr;
        }

        void setGeometry(Window* window, const Rect<int>& rect, Flags<PositionerFlag> flag) noexcept
        {
            uint32 win32Flag = toWin32Flag(flag);

            hdwp = ::DeferWindowPos(
                hdwp,
                window->getNativeHandle().as<::HWND>(),
                nullptr,
                rect.x,
                rect.y,
                rect.width,
                rect.height,
                win32Flag
            );
        }

      private:
        uint32 toWin32Flag(Flags<PositionerFlag> flag) noexcept
        {
            uint32 result = 0;
            if (flag.has(PositionerFlag::NoZOrder))
            {
                result |= SWP_NOZORDER;
            }
            if (flag.has(PositionerFlag::NoActivate))
            {
                result |= SWP_NOACTIVATE;
            }
            if (flag.has(PositionerFlag::FrameChanged))
            {
                result |= SWP_FRAMECHANGED;
            }
            if (flag.has(PositionerFlag::ShowWindow))
            {
                result |= SWP_SHOWWINDOW;
            }
            if (flag.has(PositionerFlag::NoMove))
            {
                result |= SWP_NOMOVE;
            }
            if (flag.has(PositionerFlag::NoSize))
            {
                result |= SWP_NOSIZE;
            }
            return result;
        }

      private:
        ::HDWP hdwp = nullptr;
    };

    WindowPositionerBatch::WindowPositionerBatch() noexcept : impl(std::make_unique<WindowPositionerBatchImpl>()) {}

    WindowPositionerBatch::~WindowPositionerBatch() = default;

    void WindowPositionerBatch::beginBatch(uint8 count) noexcept
    {
        impl->beginBatch(count);
    }

    void WindowPositionerBatch::endBatch() noexcept
    {
        impl->endBatch();
    }
    void WindowPositionerBatch::setGeometry(Window* window, const Rect<int>& rect, Flags<PositionerFlag> flag) noexcept
    {
        impl->setGeometry(window, rect, flag);
    }
}; // namespace gui