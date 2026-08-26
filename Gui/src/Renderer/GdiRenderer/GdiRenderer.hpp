#pragma once

#include <Windows.h>

#include "Renderer/Renderer.hpp"

namespace Gdiplus
{
    class Image;
    class GraphicsPath;
};

namespace Renderer
{
    class GdiRenderer final : public Renderer
    {
    public:
        GdiRenderer();
        ~GdiRenderer();

        void beginFrame(const gui::Window& window) override;
        void endFrame() override;

        void clear(const Color& color) override;
        void drawRect(const Rect<int>& rect, const Color& color) override;
        void drawRoundedRect(const Rect<int>& rect, const Color& color, int radius) override;

        void drawText(
            const std::string& text,
            const TextPosition& position,
            const Color& color
        ) override;

        void drawImage(const std::filesystem::path& path, const Rect<int>& rect) override;
        
    private:
        void drawImage(Gdiplus::Image* image, const Rect<int>& rect);
        Gdiplus::GraphicsPath* getRoundedRectanglePath(const Rect<int>& bounds, int radius);

        ::HDC hdc   = nullptr;
        ::HWND hwnd = nullptr;
        ::PAINTSTRUCT ps;
        ::HDC memDC         = nullptr;
        ::HBITMAP memBitmap = nullptr;
        ::HBITMAP oldBitmap = nullptr;
        ::HDC screenDC      = nullptr;
        int bufW = 0;
        int bufH = 0;

        ULONG_PTR gdiplusToken = 0;
    };

}; // namespace Renderer
