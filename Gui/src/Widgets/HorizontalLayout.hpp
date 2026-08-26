#pragma once

#include "Layout.hpp"

namespace gui
{
    class HorizontalLayout : public Layout
    {
      public:
        GUI_WIDGET_MOVABLE(HorizontalLayout)
        REGISTER_WIDGET_CLASS_NAME(HorizontalLayout)

        using Layout::Layout;

        template <typename... Args> HorizontalLayout(Args&&... args) : Layout(std::forward<Args>(args)...) {}

        HorizontalLayout(HorizontalLayout&&) noexcept            = default;
        HorizontalLayout& operator=(HorizontalLayout&&) noexcept = default;

        HorizontalLayout(const HorizontalLayout&)            = delete;
        HorizontalLayout& operator=(const HorizontalLayout&) = delete;

        int getPreferredHeight() const override;

        Widget* getWidgetAt(const Point<int>& pt) override;

      protected:
        void onBoundsChanged() override;
    };
}; // namespace gui