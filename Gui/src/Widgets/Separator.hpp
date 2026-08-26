#pragma once

#include "Widget.hpp"

namespace gui
{

    class Separator : public Widget
    {
      public:
        REGISTER_WIDGET_CLASS_NAME(Separator)
        GUI_WIDGET_MOVABLE(Separator)

        Separator() : Widget() {}
        void onPaint(Renderer::Renderer& r) override;
    };
}; // namespace gui
