#include "Font.hpp"

namespace Renderer
{
    Font::Font(std::string_view fontName, const uint16 size, const uint16 weight) noexcept
        : fontName(fontName)
        , size(size)
        , weightValue(weight)
    {
    }

    Font::~Font() noexcept
    {
    }
}