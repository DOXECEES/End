#include "GdiRenderer.hpp"

#include <WideStringUtils.hpp>

#include "Window/Window.hpp"

namespace Renderer
{
    static inline ::RECT toWin32Rect(const Rect<int>& rect) noexcept
    {
        return { rect.x, rect.y, rect.x + rect.width, rect.y + rect.height };
    }

    static inline COLORREF toGdiColor(const Color& color) noexcept
    {
        return RGB(color.r, color.g, color.b);
    }

    static inline UINT toGdiTextFormatFlags(const TextPosition& position) noexcept
    {
        UINT flags = 0;
        switch (position.alignment)
        {
        case TextAlignment::Left:
            flags |= DT_LEFT;
            break;
        
        case TextAlignment::Center:
            flags |= DT_CENTER;
            break;
        
        case TextAlignment::Right:
            flags |= DT_RIGHT;
            break;
        default:
            break;
        }

        return flags;
    }

    void GdiRenderer::beginFrame(const gui::Window& window)
    {
        hwnd     = window.getNativeHandle().as<::HWND>();
        screenDC = ::GetDC(hwnd);

        ::RECT clientRect;
        ::GetClientRect(hwnd, &clientRect);
        int w = clientRect.right - clientRect.left;
        int h = clientRect.bottom - clientRect.top;
        if (w <= 0)
        {
            w = 1;
        }
        if (h <= 0)
        {
            h = 1;
        }

        memDC     = ::CreateCompatibleDC(screenDC);
        memBitmap = ::CreateCompatibleBitmap(screenDC, w, h);
        oldBitmap = static_cast<::HBITMAP>(::SelectObject(memDC, memBitmap));

        bufW = w;
        bufH = h;

        hdc = memDC;
    }

    void GdiRenderer::endFrame()
    {
        ::BitBlt(screenDC, 0, 0, bufW, bufH, memDC, 0, 0, SRCCOPY);

        ::SelectObject(memDC, oldBitmap);
        ::DeleteObject(memBitmap);
        ::DeleteDC(memDC);
        ::ReleaseDC(hwnd, screenDC);

        hdc       = nullptr;
        memDC     = nullptr;
        memBitmap = nullptr;
        screenDC  = nullptr;
        hwnd      = nullptr;
    }

    void GdiRenderer::clear(const Color& color)
    {
        if (!hdc)
        {
            return;
        }
        ::RECT rect;
        ::GetClientRect(hwnd, &rect);

        ::COLORREF gdiColor = toGdiColor(color);
        ::HBRUSH brush      = ::CreateSolidBrush(gdiColor);
        ::FillRect(hdc, &rect, brush);
        ::DeleteObject(brush);
    }

    void GdiRenderer::drawRect(const Rect<int>& rect, const Color& color)
    {
        if (!hdc)
        {
            return;
        }

        ::RECT winRect      = toWin32Rect(rect);
        ::COLORREF gdiColor = toGdiColor(color);
        ::HBRUSH brush      = ::CreateSolidBrush(gdiColor);

        ::FillRect(hdc, &winRect, brush);
        ::DeleteObject(brush);
    }
    void GdiRenderer::drawText(
        const std::string& text,
        const TextPosition& position,
        const Color& color
    )
    {
        if (!hdc)
        {
            return;
        }

        std::wstring wideText = WideStringUtils::utf8ToWString(text);

        ::COLORREF gdiColor = toGdiColor(color);
        ::SetTextColor(hdc, gdiColor);
        ::SetBkMode(hdc, TRANSPARENT);

        ::HFONT hFont = static_cast<::HFONT>(::GetStockObject(DEFAULT_GUI_FONT));
        ::HGDIOBJ oldFont = ::SelectObject(hdc, hFont);

        const Rect<int>& rect = position.rect;

        ::RECT r_win = {
            rect.x, rect.y, rect.x + rect.width, rect.y + rect.height
        };

        ::UINT format = toGdiTextFormatFlags(position) | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
                      DT_END_ELLIPSIS;

        if (text.find('\n') != std::string::npos)
        {
            format = DT_CENTER | DT_WORDBREAK | DT_NOPREFIX;
        }

        ::DrawTextW(hdc, wideText.c_str(), -1, &r_win, format);

        ::SelectObject(hdc, oldFont);
    }
}; // namespace Renderer
