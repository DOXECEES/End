#pragma once

#include "Renderer/Font.hpp"
#include <Windows.h>

namespace Renderer
{
    class GdiFont : public Font
    {
    public:
        GdiFont(std::string_view fontName, const uint16 size, const uint16 weight) noexcept;
        ~GdiFont() noexcept override;

        GdiFont(const GdiFont&) = delete;
        GdiFont& operator=(const GdiFont&) = delete;

        GdiFont(GdiFont&& other) noexcept;
        GdiFont& operator=(GdiFont&& other) noexcept;

        [[nodiscard]] HFONT getHandle() const noexcept;

    private:
        HFONT hFont{ nullptr };
    };
}