#pragma once 

#include <Types.hpp>

#include <string_view>
#include <string>

namespace Renderer
{
    enum class FontWeight
    {
        Normal,
        Bold
    };

    class Font 
    {
    public:
        Font(std::string_view fontName, const uint16 size, const uint16 weight) noexcept;
        virtual ~Font() noexcept;

    protected:
        std::string fontName; 
        uint16 size;
        uint16 weightValue;
    };

};