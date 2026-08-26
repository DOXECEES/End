#pragma once

#include "Widget.hpp"
#include "Window/Window.hpp"
#include "Button.hpp"
#include "Application.hpp"
#include "Css/StyleSheetEngine.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

namespace gui {

    class ComboBoxItem : public Button {
    private:
        std::string itemText; 
    public:
        GUI_WIDGET_MOVABLE(ComboBoxItem)
        REGISTER_WIDGET_CLASS_NAME(ComboBoxItem)

        ComboBoxItem(const std::string& text) 
            : Button(text), itemText(text) {}

        ComboBoxItem(const ComboBoxItem&) = delete;
        ComboBoxItem& operator=(const ComboBoxItem&) = delete;

        ComboBoxItem(ComboBoxItem&&) noexcept = default;
        ComboBoxItem& operator=(ComboBoxItem&&) noexcept = default;

        void onMouseMove(const Point<int>& pt, bool isHovered) override {
            if (isHovered) {
                if (state != WidgetState::Hovered) {
                    state = WidgetState::Hovered;
                    if (getWindow()) getWindow()->repaint(true);
                }
            } else {
                if (state == WidgetState::Hovered) {
                    state = WidgetState::Normal;
                    if (getWindow()) getWindow()->repaint(true);
                }
            }
        }

        void onPaint(Renderer::Renderer& r) override {
            Color bgColor = (state == WidgetState::Hovered)
                ? Color::fromRgb(0, 120, 215)   
                : Color::fromRgb(30, 30, 30);   

            Color textColor = (state == WidgetState::Hovered)
                ? Color::fromRgb(255, 255, 255) 
                : Color::fromRgb(220, 220, 220); 

            bgColor = StyleSheetEngine::getProperty<Color>("background-color", *this, bgColor);
            textColor = StyleSheetEngine::getProperty<Color>("text-color", *this, textColor);

            r.drawRect(bounds, bgColor);

            Padding padding = StyleSheetEngine::getProperty<Padding>("padding", *this);
            int leftPadding = (padding.left > 0) ? padding.left : 12;

            Rect<int> textRect = { 
                bounds.x + leftPadding, 
                bounds.y, 
                (std::max)(0, bounds.width - leftPadding - padding.right), 
                bounds.height 
            };
            Renderer::TextPosition textPos = { textRect, Renderer::TextAlignment::Left };
            r.drawText(itemText, textPos, textColor);
        }
    };

    class ComboBox : public Widget {
    public:
        GUI_WIDGET_MOVABLE(ComboBox)
        REGISTER_WIDGET_CLASS_NAME(ComboBox)

        ComboBox(const ComboBox&) = delete;
        ComboBox& operator=(const ComboBox&) = delete;

        ComboBox(ComboBox&&) noexcept = default;
        ComboBox& operator=(ComboBox&&) noexcept = default;

        ComboBox(std::vector<std::string> items, const std::string& placeholder = "Выберите...")
            : items(std::move(items)), placeholder(placeholder)
        {
            fixedHeight(30); 
        }

        ~ComboBox() override {
            closeDropdown();
        }

        void onPaint(Renderer::Renderer& r) override 
        {
            Color bgColor = Color::fromRgb(255, 255, 255);
            Color borderColor = Color::fromRgb(180, 180, 180);
            Color textColor = Color::fromRgb(30, 30, 30);

            bgColor = StyleSheetEngine::getProperty<Color>("background-color", *this, bgColor);
            borderColor = StyleSheetEngine::getProperty<Color>("border-color", *this, borderColor);
            textColor = StyleSheetEngine::getProperty<Color>("text-color", *this, textColor);

            if (isOpened) {
                borderColor = Color::fromRgb(0, 122, 204); 
            }

            int borderRadius = StyleSheetEngine::getProperty<int>("border-radius", *this, 0);

            if (borderRadius > 0) {
                r.drawRoundedRect(bounds, borderColor, borderRadius);
                Rect<int> innerRect = { bounds.x + 1, bounds.y + 1, bounds.width - 2, bounds.height - 2 };
                r.drawRoundedRect(innerRect, bgColor, (std::max)(0, borderRadius - 1));
            } else {
                r.drawRect(bounds, borderColor);
                Rect<int> innerRect = { bounds.x + 1, bounds.y + 1, bounds.width - 2, bounds.height - 2 };
                r.drawRect(innerRect, bgColor);
            }

            Padding padding = StyleSheetEngine::getProperty<Padding>("padding", *this);

            const int arrowWidth = 16; 

            int arrowX = bounds.x + bounds.width - padding.right - arrowWidth;
            int arrowY = bounds.y + padding.top;
            int arrowH = bounds.height - padding.top - padding.bottom;
            
            Rect<int> arrowRect = { arrowX, arrowY, arrowWidth, (std::max)(0, arrowH) };
            Renderer::TextPosition arrowPos = { arrowRect, Renderer::TextAlignment::Center };
            r.drawText("\xe2\x96\xbc", arrowPos, textColor); 

            std::string displayText = (selectedIndex >= 0) ? items[selectedIndex] : placeholder;
            
            int textX = bounds.x + padding.left;
            int textY = bounds.y + padding.top;
            
            int textW = arrowX - 8 - textX;
            int textH = bounds.height - padding.top - padding.bottom;

            Rect<int> textRect = { textX, textY, (std::max)(0, textW), (std::max)(0, textH) };
            Renderer::TextPosition textPos = { textRect, Renderer::TextAlignment::Left };
            
            r.drawText(displayText, textPos, (selectedIndex >= 0) ? textColor : Color::fromRgb(150, 150, 150));
        }

        bool onMouseButtonDown(const Point<int>& pt) override {
            if (isOpened) {
                closeDropdown();
            } else {
                openDropdown();
            }
            return true; 
        }

        void onMouseMove(const Point<int>& pt, bool isHovered) override {
            if (isHovered) {
                if (state != WidgetState::Hovered && !isOpened) {
                    state = WidgetState::Hovered;
                }
            } else {
                if (state == WidgetState::Hovered) {
                    state = WidgetState::Normal;
                }
            }
        }

        void setSelectedIndex(int index) noexcept {
            if (index >= -1 && index < static_cast<int>(items.size())) {
                selectedIndex = index;
                if (m_onSelectionChangedCallback) {
                    m_onSelectionChangedCallback(selectedIndex);
                }
                if (getWindow()) {
                    getWindow()->repaint(true);
                }
            }
        }

        int getSelectedIndex() const noexcept { return selectedIndex; }
        
        const std::string& getSelectedText() const {
            if (selectedIndex >= 0) return items[selectedIndex];
            return placeholder;
        }


        void onSelectionChanged(std::function<void(int)> callback) noexcept {
            m_onSelectionChangedCallback = std::move(callback);
        }

    private:
        void openDropdown() {
            if (!getWindow() || items.empty()) return;

            isOpened = true;

            Point<int> screenPos = getWindow()->mapToScreen({ bounds.x, bounds.y + bounds.height });

            int itemHeight = 25;
            int dropdownHeight = static_cast<int>(items.size()) * itemHeight;

            dropdownWindow = std::make_unique<Window>(bounds.width, dropdownHeight, "");
            
            dropdownWindow->setStyleBatch(WindowStyles::PopUp);
            dropdownWindow->addNoActivate(); 
            dropdownWindow->setTopMost(true);

            dropdownWindow->setGeometry({screenPos.x,
                screenPos.y,
                bounds.width,
                dropdownHeight
            });

            dropdownWindow->show();
            
            dropdownWindow->setCapture();

            dropdownWindow->onMouseButtonDown([this](const Point<int>& pos) {
                Rect<int> rc = dropdownWindow->getClientRect();
                bool inside = pos.x >= 0 && pos.y >= 0 && pos.x < rc.width && pos.y < rc.height;
                if (!inside) {
                    closeDropdown();
                }
            });
            
            auto layout = std::make_unique<VerticalLayout>();
            layout->addClass("dropdown-layout"); 

            for (size_t i = 0; i < items.size(); ++i) {
                auto itemBtn = std::make_unique<ComboBoxItem>(items[i]);
                itemBtn->onClick([this, i]() {
                    setSelectedIndex(static_cast<int>(i));
                    closeDropdown();
                });
                layout->addChild(std::move(itemBtn));
            }

            dropdownWindow->setContent(std::move(layout));
        }

        void closeDropdown()
        {
            if (isOpened && dropdownWindow) 
            {
                isOpened = false;

                dropdownWindow->releaseCapture();
                dropdownWindow->hide();

                Window* raw = dropdownWindow.release(); 
                raw->requestClose();
            }
        }

    private:
        std::vector<std::string> items;
        std::string placeholder;
        int selectedIndex = -1;
        bool isOpened = false;

        std::unique_ptr<Window> dropdownWindow; 
        std::function<void(int)> m_onSelectionChangedCallback;
    };
}