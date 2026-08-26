#pragma once

#include "Css/StyleSheetEngine.hpp"

#include <memory>
#include <vector>


namespace Renderer
{
    class Renderer;
};

namespace gui
{
    class Window;
    class ApplicationImpl; 

    class Application
    {
      public:
        Application(std::shared_ptr<Renderer::Renderer> renderer) noexcept;
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
    };

}; // namespace gui