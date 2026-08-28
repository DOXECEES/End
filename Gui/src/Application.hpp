#pragma once

#include "Renderer/FontStore.hpp"
#include "Css/StyleSheetEngine.hpp"

#include <memory>
#include <vector>


namespace Renderer
{
    class Renderer;
    class FontStore;
};

namespace gui
{
    class Window;
    class ApplicationImpl; 
    class GraphicsFactory;

    class Application
    {
      public:
        Application(std::unique_ptr<GraphicsFactory> graphicsFactory) noexcept;
        ~Application();

        static Application* instance() noexcept;


        Application(const Application&)            = delete;
        Application& operator=(const Application&) = delete;
        void registerWindow(Window& window) noexcept;

        void run() noexcept;

        void setStyleSheet(StyleSheet&& localStyleSheet) noexcept;

        Renderer::Renderer* getRenderer() const noexcept;

        size_t getCountOfWindows() const noexcept;

      private:
        inline static Application* staticInstance;
        StyleSheetEngine styleSheetEngine;


        std::unique_ptr<ApplicationImpl> impl;
        std::unique_ptr<GraphicsFactory> graphicsFactory;
        std::shared_ptr<Renderer::FontStore> fontStore;

    };

}; // namespace gui