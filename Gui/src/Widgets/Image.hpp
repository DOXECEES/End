#pragma once

#include "Widget.hpp"

#include <filesystem>

namespace gui
{
    class Image : public Widget
    {
    public:
        GUI_WIDGET_MOVABLE(Image)
        REGISTER_WIDGET_CLASS_NAME(Image)

        Image(const std::filesystem::path& pathToImage)
            : path(pathToImage)
        {

        }

        void onPaint(Renderer::Renderer& r) override;

      private:
        std::filesystem::path path;
    };

};