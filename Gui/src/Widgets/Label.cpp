#include "Label.hpp"

#include "Css/StyleSheetEngine.hpp"
#include "Widget.hpp"

namespace gui
{

    void Label::onPaint(Renderer::Renderer& r)
    {
        Widget::onPaint(r);
        Color textColor = StyleSheetEngine::getProperty<Color>("text-color", *this, Color::fromRgb(40, 40, 40));
        auto font = StyleSheetEngine::getProperty<std::shared_ptr<Renderer::Font>>("font", *this);

        Padding padding = StyleSheetEngine::getProperty<Padding>("padding", *this);

        int contentX = bounds.x + padding.left;
        int contentY = bounds.y + padding.top;
        int contentW = bounds.width - padding.left - padding.right;
        int contentH = bounds.height - padding.top - padding.bottom;

        contentW = (std::max)(0, contentW);
        contentH = (std::max)(0, contentH);

        Renderer::TextPosition pos = { { contentX, contentY, contentW, contentH }, Renderer::TextAlignment::Center };
        r.drawText(text, pos, textColor, *font);
    }
    void Label::setText(const std::string& localText) noexcept
    {
        text = localText;
    }

}; // namespace gui