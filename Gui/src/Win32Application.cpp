// ApplicationWin32.cpp
#include "Application.hpp"

#include <Windows.h>
#include <memory>

#include "Renderer/GraphicsFactory.hpp"
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
        std::unique_ptr<GraphicsFactory> graphicsFactory
    ) noexcept
        : impl(std::make_unique<ApplicationImpl>())
        , graphicsFactory(std::move(graphicsFactory))
    {
        impl->renderer = this->graphicsFactory->createRenderer();
        fontStore = std::make_shared<Renderer::FontStore>(*this->graphicsFactory);
        staticInstance = this;
        styleSheetEngine.setFontStore(fontStore);
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
    void Application::setStyleSheet(StyleSheet&& localStyleSheet) noexcept
    {
        styleSheetEngine.setStyleSheet(std::move(localStyleSheet));
        for (auto* window : impl->windows)
        {
            window->repaint(true);
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