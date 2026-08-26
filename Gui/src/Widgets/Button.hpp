#pragma once
#include <Point.hpp>
#include <Rect.hpp>
#include <Types.hpp>


#include "Renderer/Renderer.hpp"
#include "Window/Window.hpp"


#include "Css/StyleSheetEngine.hpp"
#include "Layout.hpp"
#include "Widget.hpp"


namespace gui
{

    class VerticalLayout : public Layout
    {
      public:
        GUI_WIDGET_MOVABLE(VerticalLayout)
        REGISTER_WIDGET_CLASS_NAME(VerticalLayout)

        using Layout::Layout;

        template <typename... Args> VerticalLayout(Args&&... args) : Layout(std::forward<Args>(args)...) {}

        VerticalLayout(VerticalLayout&&) noexcept            = default;
        VerticalLayout& operator=(VerticalLayout&&) noexcept = default;

        VerticalLayout(const VerticalLayout&)            = delete;
        VerticalLayout& operator=(const VerticalLayout&) = delete;

        int getPreferredHeight() const override
        {
            if (children.empty()) return 0;
            int total = 0;
            for (auto& child : children)
            {
                total += child->getPreferredHeight();
            }
            int gap = StyleSheetEngine::getProperty<int>("gap", *this, 0);
            Padding padding = StyleSheetEngine::getProperty<Padding>("padding", *this, Padding{});
            int totalGaps = static_cast<int>(children.size() - 1) * gap;
            return total + totalGaps + padding.top + padding.bottom;
        }

        Widget* getWidgetAt(const Point<int>& pt) override 
        {
            
            if (pt.x < bounds.x || pt.y < bounds.y || 
                pt.x >= bounds.x + bounds.width || pt.y >= bounds.y + bounds.height) 
            {
                return nullptr;
            }

            
            Point<int> localPt = { pt.x - bounds.x, pt.y - bounds.y };

            for (auto it = children.rbegin(); it != children.rend(); ++it) 
            {
                if (Widget* found = (*it)->getWidgetAt(localPt)) 
                {
                    return found;
                }
            }

            return this; 
        }

      protected:
        void onBoundsChanged() override
        {
            if (children.empty())
            {
                return;
            }

            const int gap           = StyleSheetEngine::getProperty<int>("gap", *this, 0);
            const Padding padding    = StyleSheetEngine::getProperty<Padding>("padding", *this, Padding{});


            int totalFixed      = 0;
            int totalFlexFactor = 0;

            for (auto& child : children)
            {
                const WidgetSizePolicy& sp = child->getVerticalSizePolicy();
                switch (sp.policy)
                {
                case SizePolicy::Fixed:
                case SizePolicy::Auto:
                    totalFixed += sp.value;
                    break;
                case SizePolicy::Flex:
                    totalFlexFactor += sp.value;
                    break;
                }
            }

            const int totalGaps = static_cast<int>(children.size() - 1) * gap;

            const int usableHeight = bounds.height - padding.top - padding.bottom - totalGaps;

            const int stretchUnit = totalFlexFactor != 0
                                        ? (std::max)(1, (usableHeight - totalFixed) / totalFlexFactor)
                                        : 1;

            int currentY = bounds.y + padding.top;
            
            const int childWidth = bounds.width - padding.left - padding.right;

            for (auto& child : children)
            {
                const WidgetSizePolicy& sp = child->getVerticalSizePolicy();
                int h                      = 0;
                switch (sp.policy)
                {
                case SizePolicy::Fixed:
                case SizePolicy::Auto:
                    h = sp.value;
                    break;
                case SizePolicy::Flex:
                    h = sp.value * stretchUnit;
                    break;
                }
                
                child->setBounds({ bounds.x + padding.left, currentY, childWidth, h });
                currentY += h + gap;
            }
        }
    };

    
} // namespace gui

namespace gui
{

    class Button : public Widget
    {
      public:
        GUI_WIDGET_MOVABLE(Button)
        REGISTER_WIDGET_CLASS_NAME(Button)

        Button(const std::string& localText) : text(localText) {}

        Button& onClick(std::function<void()> callback) & noexcept;

        Button&& onClick(std::function<void()> callback) && noexcept;

        void onPaint(Renderer::Renderer& r) override;

        bool onMouseButtonDown(const Point<int>& pt) override;

        bool onMouseButtonUp(const Point<int>& pt) override;

        void onMouseMove(const Point<int>& pt, bool isHovered) override;

      private:
        std::string text;
        std::function<void()> onClickCallback;        
    };

} // namespace gui