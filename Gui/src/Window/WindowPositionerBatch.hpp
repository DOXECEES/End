#pragma once

#include <Types.hpp>
#include <Rect.hpp>
#include <Flags.hpp>

#include <memory>

namespace gui
{
    class Window;
    class WindowPositionerBatchImpl;

    enum class PositionerFlag
    {
        None = 0,
        NoZOrder = 1 << 0,
        NoActivate = 1 << 1,
        NoMove = 1 << 2,
        NoSize = 1 << 3,
        ShowWindow = 1 << 4,
        FrameChanged = 1 << 5,         
    };

    DEFINE_ENUM_FLAG_OPERATORS(PositionerFlag);


    class WindowPositionerBatch
    {
    public:
        WindowPositionerBatch() noexcept;
        ~WindowPositionerBatch();

        void beginBatch(uint8 count) noexcept;
        void endBatch() noexcept;

        void setGeometry(Window* window, const Rect<int>& rect, Flags<PositionerFlag> flag) noexcept;

    private:
        std::unique_ptr<WindowPositionerBatchImpl> impl;

    };
};