#include "GdiGraphicsFactory.hpp"

#include "GdiRenderer.hpp"
#include "GdiFont.hpp"

namespace gui
{

    std::unique_ptr<Renderer::Renderer> GdiGraphicsFactory::createRenderer() noexcept
    {
        return std::make_unique<Renderer::GdiRenderer>();
    }
    std::unique_ptr<Renderer::Font>
    GdiGraphicsFactory::createFont(std::string_view fontName, const uint16 size, const uint16 weight) noexcept
    {
        return std::make_unique<Renderer::GdiFont>(fontName, 12, weight);
    }
};