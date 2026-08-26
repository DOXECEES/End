#include "HorizontalLayout.hpp"

namespace gui
{

    int HorizontalLayout::getPreferredHeight() const
    {
        if (children.empty())
        {
            return 0;
        }
        int maxChildHeight = 0;
        for (auto& child : children)
        {
            maxChildHeight = (std::max)(maxChildHeight, child->getPreferredHeight());
        }
        Padding padding = StyleSheetEngine::getProperty<Padding>("padding", *this, Padding {});
        return maxChildHeight + padding.top + padding.bottom;
    }
    Widget* HorizontalLayout::getWidgetAt(const Point<int>& pt)
    {
        if (pt.x < bounds.x || pt.y < bounds.y || pt.x >= bounds.x + bounds.width ||
            pt.y >= bounds.y + bounds.height)
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
    void HorizontalLayout::onBoundsChanged()
    {
        if (children.empty())
        {
            return;
        }

        const int gap         = StyleSheetEngine::getProperty<int>("gap", *this, 0);
        const Padding padding = StyleSheetEngine::getProperty<Padding>("padding", *this, Padding {});

        int totalFixed      = 0;
        int totalFlexFactor = 0;

        for (size_t i = 0; i < children.size(); i++)
        {
            const WidgetSizePolicy& widgetSizePolicy = children[i]->getHorizontalSizePolicy();
            switch (widgetSizePolicy.policy)
            {
            case SizePolicy::Fixed:
            case SizePolicy::Auto:
                totalFixed += widgetSizePolicy.value;
                break;
            case SizePolicy::Flex:
                totalFlexFactor += widgetSizePolicy.value;
                break;
            default:
                break;
            }
        }

        const int totalGaps = static_cast<int>(children.size() - 1) * gap;

        const int usableWidth = bounds.width - padding.left - padding.right - totalGaps;

        const int stretchUnit = totalFlexFactor != 0 ? (std::max)(1, (usableWidth - totalFixed) / totalFlexFactor) : 1;

        int currentX = bounds.x + padding.left;

        const int childHeight = bounds.height - padding.top - padding.bottom;

        for (size_t i = 0; i < children.size(); i++)
        {
            const WidgetSizePolicy& widgetSizePolicy = children[i]->getHorizontalSizePolicy();
            int w                                    = 0;
            switch (widgetSizePolicy.policy)
            {
            case SizePolicy::Fixed:
            case SizePolicy::Auto:
                w = widgetSizePolicy.value;
                break;
            case SizePolicy::Flex:
                w = widgetSizePolicy.value * stretchUnit;
                break;
            default:
                break;
            }

            children[i]->setBounds({ currentX, bounds.y + padding.top, w, childHeight });
            currentX += w + gap;
        }
    }

}; // namespace gui