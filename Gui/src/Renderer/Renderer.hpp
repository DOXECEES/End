#pragma once

#include <Color.hpp>
#include <Rect.hpp>
#include <Types.hpp>

#include "Font.hpp"

#include <string>
#include <filesystem>

namespace gui
{
    class Window;
};

namespace Renderer
{
    enum class TextAlignment
    {
        Left,
        Center,
        Right
    };

    struct TextPosition
    {
        Rect<int> rect;
        TextAlignment alignment = TextAlignment::Left;
    };
    class Renderer
    {
      public:
        virtual ~Renderer() = default;

        virtual void beginFrame(const gui::Window& window) = 0;
        virtual void endFrame()                            = 0;

        virtual void clear(const Color& color)                           = 0;
        virtual void drawRect(const Rect<int>& rect, const Color& color) = 0;
        virtual void drawRoundedRect(const Rect<int>& rect, const Color& color, int radius) = 0;

        virtual void drawText(
            const std::string& text, const TextPosition& position, const Color& color, const Font& font
        ) = 0;

        virtual void drawImage(const std::filesystem::path& path, const Rect<int>& rect) = 0;
    };
}; // namespace Renderer
