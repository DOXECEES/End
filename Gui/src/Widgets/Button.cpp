#include "Button.hpp"

namespace gui
{

    Button& Button::onClick(std::function<void()> callback) & noexcept
    {
        onClickCallback = std::move(callback);
        return *this;
    }
    Button&& Button::onClick(std::function<void()> callback) && noexcept
    {
        onClickCallback = std::move(callback);
        return std::move(*this);
    }
    void Button::onPaint(Renderer::Renderer& r)
    {
        Widget::onPaint(r);
        Color color     = StyleSheetEngine::getProperty<Color>("background-color", *this);
        Color textColor = StyleSheetEngine::getProperty<Color>("text-color", *this);

        int borderRadius = StyleSheetEngine::getProperty<int>("border-radius", *this);

        if (borderRadius > 0)
        {
            r.drawRoundedRect(bounds, color, borderRadius);
        }
        else
        {
            r.drawRect(bounds, color);
        }

        Padding padding = StyleSheetEngine::getProperty<Padding>("padding", *this);

        int contentX = bounds.x + padding.left;
        int contentY = bounds.y + padding.top;
        int contentW = bounds.width - padding.left - padding.right;
        int contentH = bounds.height - padding.top - padding.bottom;

        contentW = (std::max)(0, contentW);
        contentH = (std::max)(0, contentH);

        Renderer::TextPosition pos = {
            { contentX, contentY, contentW, contentH },
            Renderer::TextAlignment::Center
        };
        r.drawText(text, pos, textColor);
    }
    bool Button::onMouseButtonDown(const Point<int>& pt)
    {
        state = WidgetState::Pressed;
        return true;
    }
    bool Button::onMouseButtonUp(const Point<int>& pt)
    {
        if (state == WidgetState::Pressed && onClickCallback && bounds.contains(pt))
        {
            onClickCallback();
        }
        if (state == WidgetState::Pressed && !bounds.contains(pt))
        {
            state = WidgetState::Normal;
        }
        else
        {
            state = WidgetState::Hovered;
        }
        return true;
    }

    void Button::onMouseMove(const Point<int>& pt, bool isHovered)
    {
        if (state == WidgetState::Pressed)
        {
            return;
        }
        state = isHovered ? WidgetState::Hovered : WidgetState::Normal;
    }

}; // namespace gui