#include "GraphicsFactory.hpp"

namespace gui
{
    std::unique_ptr<Renderer::Font>
    GraphicsFactory::createFont(std::string_view fontName, Renderer::FontWeight weight) noexcept
    {
        return createFont(fontName, 12, static_cast<uint16>(weight));
    }
}; // namespace gui