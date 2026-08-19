#pragma once

#include <Flags.hpp>
#include <Point.hpp>

#include <memory>
#include <string>
#include <functional>

#include "Renderer/Renderer.hpp"

namespace gui
{
    class Widget;
    enum class EventType
    {
        Close,
        KeyPress,
        Resize,
        LButtonDown,
        LButtonUp,
        MouseMove,
        SetCursor,
        Moving,
        EnterSizeMove,
        ExitSizeMove
    };

    struct Event
    {
        EventType type;
        int keyCode;
        Point<int> mousePos{0, 0}; 

    };

    class WindowImpl;

    class NativeHandle
    {
    public:
        NativeHandle(void* handle) 
            : handle(handle)
        {}

        template <typename T>
        T as() noexcept
        {
            return static_cast<T>(handle);
        }

    private:
        void* handle;
    };

    enum class WindowDataPointerType
    {
        UserData,
    };

    enum class WindowStyles
    {
        PopUp        = 1 << 0, // 1
        Caption      = 1 << 1, // 2
        ThickFrame   = 1 << 2, // 4
        SystemMenu   = 1 << 3, // 8
        Visible      = 1 << 4, // 16
        Child        = 1 << 5, // 32
        ClipSiblings = 1 << 6  // 64
    };


    DEFINE_ENUM_FLAG_OPERATORS(WindowStyles) 


    class Window
    {
      public:
        Window(int width, int height, const std::string& title, Window* parent = nullptr);
        ~Window();

        void hide() const noexcept;
        void show() const noexcept;

        bool pollEvent(Event& event);
        void update();
        bool isOpen() const;

        void setParent(Window* parent) noexcept;
        void setStyleBatch(Flags<WindowStyles> styles) noexcept;

        void addTransparency(uint8_t value) noexcept;
        void addTransparencyForInput() noexcept;

        void setTopMost(bool flag) noexcept;
        void skipTaskbar() noexcept;

        void onPaint(std::function<void(Renderer::Renderer&)> callback) noexcept;
        void onSize(std::function<void(int, int)> callback) noexcept;
        void onMouseButtonDown(std::function<void(const Point<int>&)> callback) noexcept;
        void onMouseMove(std::function<void(const Point<int>&)> callback) noexcept;
        void onMouseButtonUp(std::function<void(const Point<int>&)> callback) noexcept;
        void onMoving(std::function<void()> callback) noexcept;
        void onExitSizeMove(std::function<void()> callback) noexcept;


        NativeHandle getNativeHandle() const noexcept;

        void setRenderer(Renderer::Renderer& renderer) noexcept;

        void repaint(bool eraseBackground) noexcept;

        Rect<int> getClientRect() const noexcept; 

        void* getWindowPointerData(WindowDataPointerType type) const noexcept;
        void setWindowPointerData(WindowDataPointerType type, void* data) noexcept;

        void startSystemMove(const Point<int>& cursorPos) noexcept;

        void setGeometry(const Rect<int>& rect) noexcept;

        Point<int> mapFromScreen(const Point<int>& point) const noexcept;
        Point<int> mapToScreen(const Point<int>& point) const noexcept;

        void setCapture() noexcept;
        void releaseCapture() noexcept;

        [[nodiscard]] const std::string& getTitle() const noexcept; 
        void setTitle(const std::string& title) noexcept;

        void setContent(std::unique_ptr<Widget> localRootWidget) noexcept;
        const std::unique_ptr<Widget>& getRoot() const noexcept;

      private:
        std::unique_ptr<Widget> rootWidget; 
        std::unique_ptr<WindowImpl> impl;
        std::string title;
    };

} // namespace gui