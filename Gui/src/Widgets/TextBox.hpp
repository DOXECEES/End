// TextBox.hpp
#pragma once

#include "Widget.hpp"
#include "Css/StyleSheetEngine.hpp"

#include <string>
#include <string_view>

namespace gui {
    class TextBox : public Widget {
    public:

        GUI_WIDGET_MOVABLE(TextBox)
        REGISTER_WIDGET_CLASS_NAME(TextBox)

        TextBox(const std::string& placeholder = "") 
            : placeholderText(placeholder) 
        {
            fixedHeight(30); 
        }


        void onPaint(Renderer::Renderer& r) override
        {

            Color bgColor = StyleSheetEngine::getProperty<Color>("background-color", *this, Color::fromRgb(255, 255, 255));
            Color borderColor = StyleSheetEngine::getProperty<Color>("border-color", *this, Color::fromRgb(180, 180, 180));
            Color textColor = StyleSheetEngine::getProperty<Color>("text-color", *this, Color::fromRgb(30, 30, 30));
            auto font = StyleSheetEngine::getProperty<std::shared_ptr<Renderer::Font>>("font", *this);

            if (isFocused()) {
                borderColor = Color::fromRgb(0, 122, 204);
            }

            r.drawRect(bounds, borderColor);
            
            Rect<int> innerRect = { bounds.x + 1, bounds.y + 1, bounds.width - 2, bounds.height - 2 };
            r.drawRect(innerRect, bgColor);

            std::string textToDraw = text;
            Color finalTextColor = textColor;

            if (text.empty() && !isFocused()) {
                textToDraw = placeholderText;
                finalTextColor = Color::fromRgb(150, 150, 150);
            }

            if (isFocused()) {
                textToDraw += "|";
            }

            Rect<int> textRect = { bounds.x + 8, bounds.y, bounds.width - 16, bounds.height };
            Renderer::TextPosition pos = { textRect, Renderer::TextAlignment::Left };
            
            r.drawText(textToDraw, pos, finalTextColor, *font);
        }

        bool onMouseButtonDown(const Point<int>& pt) override {
            window->requestFocus(this);
            return true; 
        }

        void onCharInput(wchar_t ch) override {
            if (ch == L'\b') { 
                if (!text.empty()) {
                    text.pop_back();
                }
            }
            else if (ch == L'\r' || ch == L'\n') { 
            }
            else if (ch >= 32) { 
                text.push_back(ch);
            }
        }

        void onMouseMove(const Point<int>& pt, bool isHovered) override {
            if (state != WidgetState::Hovered && !isFocused())
            {
                state = WidgetState::Hovered;
            }
        }

        [[nodiscard]] const std::string& getText() const noexcept { return text; }
        void setText(const std::string& localText) noexcept { text = localText; }

        TextBox& placeholder(std::string_view localPlaceholderText) & noexcept { placeholderText = localPlaceholderText; return *this; }
        TextBox&& placeholder(std::string_view localPlaceholderText) && noexcept { placeholderText = localPlaceholderText; return std::move(*this); }

    private:
        std::string text;
        std::string placeholderText;
        bool isHovered = false;
    };
}