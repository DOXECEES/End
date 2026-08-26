#pragma once

#include "Widget.hpp"

#include <functional>

namespace gui
{
    class Checkbox : public Widget
    {
      public:
        GUI_WIDGET_MOVABLE(Checkbox)
        REGISTER_WIDGET_CLASS_NAME(Checkbox)

        Checkbox(const std::string& text) : text(text) {}

        Checkbox& onToggle(std::function<void(bool)> callback) & noexcept;

        Checkbox&& onToggle(std::function<void(bool)> callback) && noexcept;
        void onPaint(Renderer::Renderer& r) override;

        bool onMouseButtonDown(const Point<int>& pt) override;

        bool onMouseButtonUp(const Point<int>& pt) override;

        void onMouseMove(const Point<int>& pt, bool isHovered) override;

      private:
        std::string text;
        bool checked   = false;
        bool isHovered = false;
        std::function<void(bool)> onToggleCallback;
    };

}; // namespace gui