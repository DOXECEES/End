// ApplicationWin32.cpp
#include "Application.hpp"

#include <Windows.h>

#include "Window/Window.hpp"

namespace gui
{
    class ApplicationImpl
    {
      public:
        std::shared_ptr<Renderer::Renderer> renderer;
        std::vector<Window*> windows;
    };

    Application::Application(
        std::shared_ptr<Renderer::Renderer> renderer
    ) noexcept
        : impl(std::make_unique<ApplicationImpl>())
    {
        impl->renderer = std::move(renderer);
        staticInstance = this;
    }

    Application::~Application() = default;

    Application* Application::instance() noexcept
    {
        return staticInstance;
    }

    void Application::registerWindow(Window& window) noexcept
    {
        impl->windows.push_back(&window);
        window.setRenderer(*impl->renderer);
    }

    void Application::run() noexcept
    {
        MSG msg = {};
        while (GetMessageW(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    Renderer::Renderer* Application::getRenderer() const noexcept
    {
        return impl->renderer.get();
    }

    size_t Application::getCountOfWindows() const noexcept
    {
        return impl->windows.size();
    }
} // namespace gui