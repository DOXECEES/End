#pragma once

#include <Windows.h>

#include "Renderer/Renderer.hpp"

namespace Renderer
{
    class GdiRenderer final : public Renderer
    {
        void beginFrame(const gui::Window& window) override;
        void endFrame() override;

        void clear(const Color& color) override;
        void drawRect(const Rect<int>& rect, const Color& color) override;
        void drawText(
            const std::string& text,
            const TextPosition& position,
            const Color& color
        ) override;

      private:
        ::HDC hdc   = nullptr;
        ::HWND hwnd = nullptr;
        ::PAINTSTRUCT ps;
        ::HDC memDC         = nullptr;
        ::HBITMAP memBitmap = nullptr;
        ::HBITMAP oldBitmap = nullptr;
        ::HDC screenDC      = nullptr;
        int bufW = 0;
        int bufH = 0;
    };

}; // namespace Renderer
