#include "Widget.hpp"

#include "Window/Window.hpp"
#include "Css/StyleSheetEngine.hpp"

#include <utility>

namespace gui
{
    void Widget::onPaint(Renderer::Renderer& r)
    {

    };

    const WidgetSizePolicy& Widget::getVerticalSizePolicy() const noexcept
    {
        return verticalSizePolicy;
    }

    const WidgetSizePolicy& Widget::getHorizontalSizePolicy() const noexcept
    {
        return horizontalSizePolicy;
    }

    Widget& Widget::fixedHeight(int h) & noexcept
    {
        verticalSizePolicy.policy = SizePolicy::Fixed;
        verticalSizePolicy.value  = h;
        return *this;
    }

    Widget&& Widget::fixedHeight(int h) && noexcept
    {
        verticalSizePolicy.policy = SizePolicy::Fixed;
        verticalSizePolicy.value  = h;
        return std::move(*this);
    }

    Widget& Widget::autoHeight() & noexcept
    {
        verticalSizePolicy.policy = SizePolicy::Auto;
        verticalSizePolicy.value  = 0;
        return *this;
    }

    Widget&& Widget::autoHeight() && noexcept
    {
        verticalSizePolicy.policy = SizePolicy::Auto;
        verticalSizePolicy.value  = 0;
        return std::move(*this);
    }

    Widget& Widget::stretchHeight(int weight) & noexcept
    {
        verticalSizePolicy.policy = SizePolicy::Flex;
        verticalSizePolicy.value  = weight;
        return *this;
    }

    Widget&& Widget::stretchHeight(int weight) && noexcept
    {
        verticalSizePolicy.policy = SizePolicy::Flex;
        verticalSizePolicy.value  = weight;
        return std::move(*this);
    }

    Widget& Widget::fixedWidth(int w) & noexcept
    {
        horizontalSizePolicy.policy = SizePolicy::Fixed;
        horizontalSizePolicy.value  = w;
        return *this;
    }

    Widget&& Widget::fixedWidth(int w) && noexcept
    {
        horizontalSizePolicy.policy = SizePolicy::Fixed;
        horizontalSizePolicy.value  = w;
        return std::move(*this);
    }

    Widget& Widget::autoWidth() & noexcept
    {
        horizontalSizePolicy.policy = SizePolicy::Auto;
        horizontalSizePolicy.value  = 0;
        return *this;
    }

    Widget&& Widget::autoWidth() && noexcept
    {
        horizontalSizePolicy.policy = SizePolicy::Auto;
        horizontalSizePolicy.value  = 0;
        return std::move(*this);
    }

    Widget& Widget::stretchWidth(int weight) & noexcept
    {
        horizontalSizePolicy.policy = SizePolicy::Flex;
        horizontalSizePolicy.value  = weight;
        return *this;
    }

    Widget&& Widget::stretchWidth(int weight) && noexcept
    {
        horizontalSizePolicy.policy = SizePolicy::Flex;
        horizontalSizePolicy.value  = weight;
        return std::move(*this);
    }
    bool Widget::isFocused() const noexcept 
    {
        return window->getFocusedWidget() == this;
    }
    int Widget::getPreferredHeight() const
    {
        const auto& sp = getVerticalSizePolicy();
        if (sp.policy == SizePolicy::Fixed || sp.policy == SizePolicy::Auto)
        {
            return sp.value;
        }
        return bounds.height;
    }

} // namespace gui