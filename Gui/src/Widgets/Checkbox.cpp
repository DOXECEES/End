#include "Checkbox.hpp"

#include "Css/StyleSheetEngine.hpp"

namespace gui
{

    Checkbox& Checkbox::onToggle(std::function<void(bool)> callback) & noexcept
    {
        onToggleCallback = std::move(callback);
        return *this;
    }
    Checkbox&& Checkbox::onToggle(std::function<void(bool)> callback) && noexcept
    {
        onToggleCallback = std::move(callback);
        return std::move(*this);
    }
    void Checkbox::onPaint(Renderer::Renderer& r)
    {
        Widget::onPaint(r);

        auto font = StyleSheetEngine::getProperty<std::shared_ptr<Renderer::Font>>("font", *this);


        const int boxSize = 14;   
        const int paddingLeft = 4;  
        
        int boxX = bounds.x + paddingLeft;
        int boxY = bounds.y + (bounds.height - boxSize) / 2;  

        Rect<int> borderRect = { boxX, boxY, boxSize, boxSize };
        r.drawRect(borderRect, isHovered ? Color::fromRgb(0, 120, 215) : Color::fromRgb(130, 130, 130));

        Rect<int> innerBoxRect = { boxX + 1, boxY + 1, boxSize - 2, boxSize - 2 };
        r.drawRect(innerBoxRect, Color::fromRgb(255, 255, 255));

        if (checked)
        {
            Renderer::TextPosition checkPos = { innerBoxRect, Renderer::TextAlignment::Center };
            r.drawText("X", checkPos, Color::fromRgb(0, 120, 215), *font); 
        }

        int textX = boxX + boxSize + 8; 
        int textWidth = bounds.width - (textX - bounds.x);
        
        Rect<int> textRect = { textX, bounds.y, textWidth, bounds.height };
        
        Renderer::TextPosition textPos = { textRect, Renderer::TextAlignment::Left };
        r.drawText(text, textPos, Color::fromRgb(0, 0, 0), *font);
    }

    bool Checkbox::onMouseButtonDown(const Point<int>& pt)
    {
        return true;
    }
    bool Checkbox::onMouseButtonUp(const Point<int>& pt)
    {
        if (isHovered)
        {
            checked = !checked;
            if (onToggleCallback)
            {
                onToggleCallback(checked);
            }
        }
        return true;
    }
    void Checkbox::onMouseMove(const Point<int>& pt, bool localIsHovered)
    {
        isHovered = localIsHovered;
    }

}; // namespace gui