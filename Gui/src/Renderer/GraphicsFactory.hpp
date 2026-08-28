#pragma once

#include "Renderer.hpp"
#include "Font.hpp"

namespace gui
{
    class GraphicsFactory
    {
    public:
        GraphicsFactory() = default;
        virtual ~GraphicsFactory() = default;

        virtual std::unique_ptr<Renderer::Renderer> createRenderer() noexcept = 0;
        std::unique_ptr<Renderer::Font> createFont(std::string_view fontName, Renderer::FontWeight weight) noexcept;

        virtual std::unique_ptr<Renderer::Font>
        createFont(std::string_view fontName, const uint16 size, const uint16 weight) noexcept = 0;
    };

};