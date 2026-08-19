#include "Window.hpp"

#include <WideStringUtils.hpp>

#include "Application.hpp"
#include "Widgets/Widget.hpp"

#ifndef UNICODE
#define UNICODE
#endif

#include <queue>
#include <windows.h>

namespace gui
{
    LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static bool registerWindowClass(HINSTANCE hInst)
    {
        static bool registered = false;
        if (!registered)
        {
            WNDCLASSW wc     = {};
            wc.lpfnWndProc   = WndProc;
            wc.hInstance     = hInst;
            wc.lpszClassName = L"GUI_WindowClass";
            wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
            wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);

            if (RegisterClassW(&wc))
            {
                registered = true;
            }
        }
        return registered;
    }

    static constexpr uint32 toWin32WindowDataPointerType(WindowDataPointerType type)
    {
        switch (type)
        {
        case WindowDataPointerType::UserData:
        {
            return GWLP_USERDATA;
        }
        }
    }

    class WindowImpl
    {
      public:
        HWND hwnd = nullptr;
        bool open = true;
        std::queue<Event> eventQueue;

        WindowImpl(Window* ownerPtr, int width, int height, const std::string& title, Window* parentPtr)
            : owner(ownerPtr), parent(parentPtr)
        {
            HINSTANCE hInst = GetModuleHandleW(NULL);
            if (!registerWindowClass(hInst))
            {
                open = false;
                return;
            }
            uint32_t styles       = WS_CLIPCHILDREN;
            std::wstring wTitle   = WideStringUtils::utf8ToWString(title);
            Application* instance = Application::instance();
            ::HWND parentHwnd     = nullptr;
            ::HMENU menuOrId      = nullptr;

            if (parent)
            {
                parentHwnd  = parent->getNativeHandle().as<::HWND>();
                styles     |= WS_CHILD;
                menuOrId    = reinterpret_cast<HMENU>(instance->getCountOfWindows());
            }
            else
            {
                styles |= WS_OVERLAPPEDWINDOW;
            }

            hwnd = ::CreateWindowExW(
                0,
                L"GUI_WindowClass",
                wTitle.c_str(),
                styles,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                width,
                height,
                parentHwnd,
                menuOrId,
                hInst,
                this
            );

            if (hwnd)
            {
                ShowWindow(hwnd, SW_SHOW);
                UpdateWindow(hwnd);
            }
            else
            {
                open = false;
            }
        }

        ~WindowImpl()
        {
            if (hwnd)
            {
                DestroyWindow(hwnd);
            }
        }

        void setCapture() noexcept
        {
            if (hwnd)
            {
                ::SetCapture(hwnd);
            }
        }
        void releaseCapture() noexcept
        {
            ::ReleaseCapture();
        }

        bool pollEvent(Event& event)
        {
            MSG msg;
            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }

            if (!eventQueue.empty())
            {
                event = eventQueue.front();
                eventQueue.pop();
                return true;
            }
            return false;
        }

        void update()
        {
            if (hwnd)
            {
                UpdateWindow(hwnd);
            }
        }

        bool isOpen() const
        {
            return open;
        }

        void setStyleBatch(Flags<WindowStyles> styles) noexcept
        {
            ::DWORD style = 0;
            if (styles.has(WindowStyles::PopUp))
            {
                style |= WS_POPUP;
            }

            if (styles.has(WindowStyles::Caption))
            {
                style |= WS_CAPTION;
            }

            if (styles.has(WindowStyles::SystemMenu))
            {
                style |= WS_SYSMENU;
            }

            if (styles.has(WindowStyles::ThickFrame))
            {
                style |= WS_THICKFRAME;
            }

            if (styles.has(WindowStyles::Visible))
            {
                style |= WS_VISIBLE;
            }

            if (styles.has(WindowStyles::Child))
            {
                style |= WS_CHILD;
            }

            if (styles.has(WindowStyles::ClipSiblings))
            {
                style |= WS_CLIPSIBLINGS;
            }

            ::SetWindowLongPtrW(hwnd, GWL_STYLE, style);
            ::SetWindowPos(
                hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE
            );
        }

        void setParent(Window* parent)
        {
            ::HWND parentHwnd = nullptr;

            if (parent)
            {
                parentHwnd = parent->getNativeHandle().as<::HWND>();
            }

            ::HWND result = ::SetParent(hwnd, parentHwnd);
            if (result)
            {
                ::SetWindowPos(
                    hwnd,
                    HWND_TOP,
                    0,
                    0,
                    0,
                    0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_FRAMECHANGED | SWP_NOACTIVATE
                );
            }
        }

        void addTransparency(uint8_t value) noexcept
        {
            if (hwnd)
            {
                LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

                SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);

                SetLayeredWindowAttributes(hwnd, 0, value, LWA_ALPHA);
            }
        }

        void addTransparencyForInput() noexcept
        {
            if (hwnd)
            {
                LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

                SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
            }
        }

        void skipTaskbar() noexcept
        {
            if (hwnd)
            {
                LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

                SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TOOLWINDOW);
            }
        }

        void setTopMost(bool flag) noexcept
        {
            if (hwnd)
            {
                LONG_PTR exStyle = ::GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
                if (flag)
                {
                    ::SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TOPMOST);
                }
                else
                {
                    ::SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TOPMOST);
                }

                ::SetWindowPos(
                    hwnd,
                    flag ? HWND_TOPMOST : HWND_NOTOPMOST,
                    0,
                    0,
                    0,
                    0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED
                );
            }
        }

        void repaint(bool eraseBackground) noexcept
        {
            if (hwnd)
            {
                // InvalidateRect(hwnd, nullptr, eraseBackground);
                ::RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
            }
        }

        Rect<int> getClientRect() const noexcept
        {
            RECT rect {};
            GetClientRect(hwnd, &rect);
            return Rect<int> { rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top };
        }

        void hide() const noexcept
        {
            ::ShowWindow(hwnd, SW_HIDE);
        }

        void show() const noexcept
        {
            ::ShowWindow(hwnd, SW_SHOW);
        }

        void* getWindowPointerData(WindowDataPointerType type) const noexcept
        {
            if (type == WindowDataPointerType::UserData)
            {
                return userData;
            }
        }

        void startSystemMove(const Point<int>& cursorPos) noexcept
        {
            ::ReleaseCapture();
            ::PostMessageW(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(cursorPos.x, cursorPos.y));
        }

        void setWindowPointerData(WindowDataPointerType type, void* data) noexcept
        {
            if (type == WindowDataPointerType::UserData)
            {
                userData = data;
            }
        }

        void setGeometry(const Rect<int>& rect) noexcept
        {
            if (hwnd)
            {
                ::SetWindowPos(hwnd, nullptr, rect.x, rect.y, rect.width, rect.height, SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }

        Point<int> mapFromScreen(const Point<int>& point) const noexcept
        {
            if (hwnd)
            {
                POINT pt { point.x, point.y };
                ::ScreenToClient(hwnd, &pt);
                return Point<int> { pt.x, pt.y };
            }
            return point;
        }

        Point<int> mapToScreen(const Point<int>& point) const noexcept
        {
            if (hwnd)
            {
                POINT pt { point.x, point.y };
                ::ClientToScreen(hwnd, &pt);
                return Point<int> { pt.x, pt.y };
            }
            return point;
        }

        [[nodiscard]] bool setTitle(const std::string& title) noexcept
        {
            if (hwnd)
            {
                std::wstring wTitle = WideStringUtils::utf8ToWString(title);
                ::SetWindowTextW(hwnd, wTitle.c_str());
                return true;
            }
            return false;
        }

        std::function<void(Renderer::Renderer&)> paintCallback;
        std::function<void(int, int)> onSizeCallback;
        std::function<void(const Point<int>&)> mouseButtonDownCallback;
        std::function<void(const Point<int>&)> mouseMoveCallback;
        std::function<void(const Point<int>&)> mouseButtonUpCallback;
        std::function<void()> movingCallback;
        std::function<void()> exitSizeMoveCallback;

        Window* owner                = nullptr;
        Window* parent               = nullptr;
        Renderer::Renderer* renderer = nullptr;

        void* userData = nullptr;
    };

    LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        // Извлекаем указатель на WindowImpl, сохраненный при создании
        WindowImpl* pImpl = reinterpret_cast<WindowImpl*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

        if (uMsg == WM_PAINT)
        {
            char buf[128];
            sprintf_s(buf, "WM_PAINT hwnd=%p\n", (void*)hwnd);
            OutputDebugStringA(buf);
        }

        switch (uMsg)
        {
        case WM_NCCREATE:
        {
            CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            if (pImpl && pImpl->paintCallback && pImpl->renderer && pImpl->owner)
            {
                pImpl->renderer->beginFrame(*pImpl->owner);
                pImpl->paintCallback(*pImpl->renderer);
                if (pImpl->owner->getRoot())
                {
                    pImpl->owner->getRoot()->onPaint(*pImpl->renderer);
                }

                pImpl->renderer->endFrame();
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SIZE:
        {
            if (pImpl)
            {
                pImpl->eventQueue.push({ EventType::Resize, 0 });
                int w = LOWORD(lParam);
                int h = HIWORD(lParam);
                if (pImpl->onSizeCallback)
                {
                    pImpl->onSizeCallback(w, h);
                }

                if (pImpl->owner->getRoot())
                {
                    pImpl->owner->getRoot()->setBounds({ 0, 0, w, h });
                }
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            if (pImpl)
            {
                int x = static_cast<short>(LOWORD(lParam));
                int y = static_cast<short>(HIWORD(lParam));
                Point<int> mousePos { x, y };

                pImpl->eventQueue.push({ EventType::LButtonDown, 0, mousePos });

                if (pImpl->mouseButtonDownCallback)
                {
                    pImpl->mouseButtonDownCallback(mousePos);
                }

                if (pImpl->owner->getRoot())
                {
                    pImpl->owner->getRoot()->onMouseButtonDown({ x, y });
                }

                pImpl->repaint(true);
            }
            return 0;
        }

        case WM_LBUTTONUP:
        {
            if (pImpl)
            {
                int x = static_cast<short>(LOWORD(lParam));
                int y = static_cast<short>(HIWORD(lParam));
                Point<int> mousePos { x, y };

                pImpl->eventQueue.push({ EventType::LButtonUp, 0, mousePos });

                if (pImpl->mouseButtonUpCallback)
                {
                    pImpl->mouseButtonUpCallback(mousePos);
                }

                if (pImpl->owner->getRoot())
                {
                    pImpl->owner->getRoot()->onMouseButtonUp({ x, y });
                }

                pImpl->repaint(true);
            }
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (pImpl)
            {
                int x = static_cast<short>(LOWORD(lParam));
                int y = static_cast<short>(HIWORD(lParam));
                Point<int> mousePos { x, y };

                pImpl->eventQueue.push({ EventType::MouseMove, 0, mousePos });

                if (pImpl->mouseMoveCallback)
                {
                    pImpl->mouseMoveCallback(mousePos);
                }

                if (pImpl->owner->getRoot())
                {
                    pImpl->owner->getRoot()->onMouseMove({ x, y }, true);
                }

                pImpl->repaint(true);
            }
            return 0;
        }

        case WM_MOVING:
        {
            if (pImpl)
            {
                pImpl->eventQueue.push(
                    {
                        EventType::Moving, 0, { 0, 0 }
                }
                );

                // Вызов функтора
                if (pImpl->movingCallback)
                {
                    pImpl->movingCallback();
                }
            }
            break;
        }

        case WM_EXITSIZEMOVE:
        {
            if (pImpl)
            {
                pImpl->eventQueue.push(
                    {
                        EventType::ExitSizeMove, 0, { 0, 0 }
                }
                );

                if (pImpl->exitSizeMoveCallback)
                {
                    pImpl->exitSizeMoveCallback();
                }
            }
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_KEYDOWN:
        {
            if (pImpl)
            {
                pImpl->eventQueue.push({ EventType::KeyPress, static_cast<int>(wParam) });
            }
            return 0;
        }
        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_NCDESTROY:
        {
            if (pImpl)
            {
                pImpl->hwnd = nullptr;
                pImpl->open = false;
            }
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

    Window::Window(int width, int height, const std::string& title, Window* parent)
        : impl(std::make_unique<WindowImpl>(this, width, height, title, parent)), title(title)
    {
        if (Application* app = Application::instance())
        {
            app->registerWindow(*this);
        }
    }
    Window::~Window() = default;

    void Window::hide() const noexcept
    {
        impl->hide();
    }

    void Window::show() const noexcept
    {
        impl->show();
    }

    bool Window::pollEvent(Event& event)
    {
        return impl->pollEvent(event);
    }

    void Window::update()
    {
        impl->update();
    }

    bool Window::isOpen() const
    {
        return impl->isOpen();
    }

    void Window::setParent(Window* parent) noexcept
    {
        impl->setParent(parent);
    }

    void Window::setStyleBatch(Flags<WindowStyles> styles) noexcept
    {
        impl->setStyleBatch(styles);
    }

    void Window::addTransparency(uint8_t value) noexcept
    {
        impl->addTransparency(value);
    }

    void Window::addTransparencyForInput() noexcept
    {
        impl->addTransparencyForInput();
    }

    void Window::setTopMost(bool flag) noexcept
    {
        impl->setTopMost(flag);
    }

    void Window::skipTaskbar() noexcept
    {
        impl->skipTaskbar();
    }

    void Window::onPaint(std::function<void(Renderer::Renderer&)> callback) noexcept
    {
        impl->paintCallback = std::move(callback);
    }

    void Window::onSize(std::function<void(int, int)> callback) noexcept
    {
        impl->onSizeCallback = std::move(callback);
    }

    void Window::onMouseButtonDown(std::function<void(const Point<int>&)> callback) noexcept
    {
        impl->mouseButtonDownCallback = std::move(callback);
    }

    void Window::onMouseMove(std::function<void(const Point<int>&)> callback) noexcept
    {
        impl->mouseMoveCallback = std::move(callback);
    }

    void Window::onMouseButtonUp(std::function<void(const Point<int>&)> callback) noexcept
    {
        impl->mouseButtonUpCallback = std::move(callback);
    }

    void Window::onMoving(std::function<void()> callback) noexcept
    {
        impl->movingCallback = std::move(callback);
    }

    void Window::onExitSizeMove(std::function<void()> callback) noexcept
    {
        impl->exitSizeMoveCallback = std::move(callback);
    }

    NativeHandle Window::getNativeHandle() const noexcept
    {
        return impl->hwnd;
    }

    void Window::setRenderer(Renderer::Renderer& renderer) noexcept
    {
        impl->renderer = &renderer;
    }

    void Window::repaint(bool eraseBackground) noexcept
    {
        impl->repaint(eraseBackground);
    }

    Rect<int> Window::getClientRect() const noexcept
    {
        return impl->getClientRect();
    }

    void* Window::getWindowPointerData(WindowDataPointerType type) const noexcept
    {
        return impl->getWindowPointerData(type);
    }

    void Window::setWindowPointerData(WindowDataPointerType type, void* data) noexcept
    {
        impl->setWindowPointerData(type, data);
    }

    void Window::startSystemMove(const Point<int>& cursorPos) noexcept
    {
        impl->startSystemMove(cursorPos);
    }

    void Window::setGeometry(const Rect<int>& rect) noexcept
    {
        impl->setGeometry(rect);
    }

    Point<int> Window::mapFromScreen(const Point<int>& point) const noexcept
    {
        return impl->mapFromScreen(point);
    }

    Point<int> Window::mapToScreen(const Point<int>& point) const noexcept
    {
        return impl->mapToScreen(point);
    }

    void Window::setCapture() noexcept
    {
        impl->setCapture();
    }
    void Window::releaseCapture() noexcept
    {
        impl->releaseCapture();
    }

    const std::string& Window::getTitle() const noexcept
    {
        return title;
    }

    void Window::setTitle(const std::string& title) noexcept
    {
        bool isSuccess = impl->setTitle(title);
        if (!isSuccess)
        {
            return;
        }
        this->title = title;
    }

    void Window::setContent(std::unique_ptr<Widget> localRootWidget) noexcept
    {
        if (rootWidget)
        {
            rootWidget->setRootWindow(nullptr);
        }

        rootWidget = std::move(localRootWidget);

        if (rootWidget)
        {
            rootWidget->setRootWindow(this);

            Rect<int> clientRect = getClientRect();
            rootWidget->setBounds(clientRect);
        }
    }

    const std::unique_ptr<Widget>& Window::getRoot() const noexcept
    {
        return rootWidget;
    }

} // namespace gui