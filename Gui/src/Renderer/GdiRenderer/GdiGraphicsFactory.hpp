#pragma once 

#include "Renderer/GraphicsFactory.hpp"

namespace gui
{
    class GdiGraphicsFactory : public GraphicsFactory
    {
    public:
      std::unique_ptr<Renderer::Renderer> createRenderer() noexcept override;

      std::unique_ptr<Renderer::Font>
      createFont(std::string_view fontName, const uint16 size, const uint16 weight) noexcept override;

    };

};