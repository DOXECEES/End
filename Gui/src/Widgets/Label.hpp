#pragma once 

#include "Widget.hpp"

namespace gui
{
    class Label : public Widget {
    public:
        REGISTER_WIDGET_CLASS_NAME(Label)
        GUI_WIDGET_MOVABLE(Label)

        Label(const std::string& localText) : text(localText) {}

        void onPaint(Renderer::Renderer& r) override;

        void setText(const std::string& localText) noexcept;

      private:
        std::string text;
    };

};