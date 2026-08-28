#include "GdiFont.hpp"

#include "WideStringUtils.hpp"

#include <string>

namespace Renderer
{
    GdiFont::GdiFont(std::string_view fontName, const uint16 size, const uint16 weight) noexcept
        : Font(fontName, size, weight)
    {
        std::wstring wFontName = WideStringUtils::utf8ToWString(fontName.data());

        int dpiY = 96;
        HDC hdc = GetDC(nullptr);
        if (hdc)
        {
            dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
            ReleaseDC(nullptr, hdc);
        }
        
        int height = -MulDiv(size, dpiY, 72);

        hFont = CreateFontW(
            height,
            0,                          // Ширина (0 — выбор по умолчанию)
            0,                          // Угол наклона
            0,                          // Угол ориентации
            weightValue,                  // Насыщенность
            FALSE,                      // Курсив
            FALSE,                      // Подчеркивание
            FALSE,                      // Зачеркивание
            DEFAULT_CHARSET,            // Набор символов
            OUT_DEFAULT_PRECIS,         // Точность вывода
            CLIP_DEFAULT_PRECIS,        // Точность отсечения
            CLEARTYPE_QUALITY,          // Качество отрисовки
            DEFAULT_PITCH | FF_DONTCARE, // Шаг и семейство шрифта
            wFontName.c_str()           // Имя гарнитуры
        );
    }

    GdiFont::~GdiFont() noexcept
    {
        if (hFont)
        {
            DeleteObject(hFont);
        }
    }

    GdiFont::GdiFont(GdiFont&& other) noexcept
        : Font(std::move(other)), hFont(other.hFont)
    {
        other.hFont = nullptr;
    }

    GdiFont& GdiFont::operator=(GdiFont&& other) noexcept
    {
        if (this != &other)
        {
            if (hFont)
            {
                DeleteObject(hFont);
            }
            Font::operator=(std::move(other));
            hFont = other.hFont;
            other.hFont = nullptr;
        }
        return *this;
    }

    HFONT GdiFont::getHandle() const noexcept
    {
        return hFont;
    }
}