#include <Types.hpp>
#include <Rect.hpp>
#include <Point.hpp>

#include "Window/Window.hpp"
#include "Renderer/Renderer.hpp"

#include "Layout.hpp"
#include "Widget.hpp"

namespace gui {
    

    class VerticalLayout : public Layout {
    public:

        GUI_WIDGET_MOVABLE(VerticalLayout)

        using Layout::Layout;

        template <typename... Args>
        VerticalLayout(Args&&... args) : Layout(std::forward<Args>(args)...) {}


        VerticalLayout(VerticalLayout&&) noexcept = default;
        VerticalLayout& operator=(VerticalLayout&&) noexcept = default;

        VerticalLayout(const VerticalLayout&) = delete;
        VerticalLayout& operator=(const VerticalLayout&) = delete;


    protected:
        void onBoundsChanged() override {
            if (m_children.empty()) return;
            int childHeight = m_bounds.height / static_cast<int>(m_children.size());
            int currentY = m_bounds.y;

            for (auto& child : m_children) {
                child->setBounds({ m_bounds.x, currentY, m_bounds.width, childHeight });
                currentY += childHeight;
            }
        }
    };

    class HorizontalLayout : public Layout
    {
      public:
        GUI_WIDGET_MOVABLE(HorizontalLayout)
      
        using Layout::Layout;

        template <typename... Args>
        HorizontalLayout(Args&&... args) : Layout(std::forward<Args>(args)...) {}

        HorizontalLayout(HorizontalLayout&&) noexcept            = default;
        HorizontalLayout& operator=(HorizontalLayout&&) noexcept = default;

        HorizontalLayout(const HorizontalLayout&)            = delete;
        HorizontalLayout& operator=(const HorizontalLayout&) = delete;

      protected:

        void onBoundsChanged() override
        {
            if (m_children.empty())
            {
                return;
            }


            int totalFixed = 0;
            int totalFlexFactor = 0;

            for(size_t i = 0; i < m_children.size(); i++)
            {
                const WidgetSizePolicy& widgetSizePolicy = m_children[i]->getHorizontalSizePolicy();
                switch (widgetSizePolicy.policy)
                {
                case SizePolicy::Fixed:
                {
                    totalFixed = widgetSizePolicy.value;
                    break;
                }
                case SizePolicy::Auto:
                {
                    // totalFixed = calculate auto 
                    break;
                }
                case SizePolicy::Flex:
                {
                    totalFlexFactor += widgetSizePolicy.value; 
                    break;
                }
                default:
                    break;
                }
            }

            const int stretchUnit = totalFlexFactor != 0 ? (m_bounds.width - totalFixed) / totalFlexFactor : 1;

            int currentX = 0;

            for(size_t i = 0; i < m_children.size(); i++)
            {
                const WidgetSizePolicy& widgetSizePolicy = m_children[i]->getHorizontalSizePolicy();
                switch (widgetSizePolicy.policy)
                {
                    case SizePolicy::Fixed:
                    {
                        m_children[i]->setBounds({currentX, m_bounds.y, widgetSizePolicy.value, m_bounds.height});
                        currentX += widgetSizePolicy.value;

                        break;
                    }
                    case SizePolicy::Auto:
                    {
                        // totalFixed = calculate auto 
                        break;
                    }
                    case SizePolicy::Flex:
                    {
                        m_children[i]->setBounds({currentX, m_bounds.y, widgetSizePolicy.value * stretchUnit, m_bounds.height});
                        currentX += widgetSizePolicy.value * stretchUnit;
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    };
}

namespace gui {
    class Label : public Widget {
    public:
        GUI_WIDGET_MOVABLE(Label)

        Label(const std::string& text) : m_text(text) {}

        void onPaint(Renderer::Renderer& r) override {
            Renderer::TextPosition pos = {
                m_bounds,
                Renderer::TextAlignment::Center
            };
            r.drawText(m_text, pos, Color::fromRgb(40, 40, 40));
        }

        void setText(const std::string& text) noexcept {
            m_text = text;
        }

    private:
        std::string m_text;
    };

    class Button : public Widget {
    public:
        GUI_WIDGET_MOVABLE(Button)

        Button(const std::string& text) : m_text(text) {}

        Button& onClick(std::function<void()> callback) & noexcept 
        {
            m_onClick = std::move(callback);
            return *this;
        }

        Button&& onClick(std::function<void()> callback) && noexcept 
        {
            m_onClick = std::move(callback);
            return std::move(*this); 
        }

        void onPaint(Renderer::Renderer& r) override {
            Color color = Color::fromRgb(220, 220, 220);
            if (m_isPressed) {
                color = Color::fromRgb(160, 160, 160);
            } else if (m_isHovered) {
                color = Color::fromRgb(240, 240, 240);
            }
            r.drawRect(m_bounds, color);
            Renderer::TextPosition pos = {
                m_bounds,
                Renderer::TextAlignment::Center
            };
            r.drawText(m_text, pos, Color::fromRgb(0, 0, 0));
        }

        bool onMouseButtonDown(const Point<int>& pt) override {
            m_isPressed = true;
            return true;
        }

        bool onMouseButtonUp(const Point<int>& pt) override {
            if (m_isPressed && m_isHovered && m_onClick) {
                m_onClick();
            }
            m_isPressed = false;
            return true;
        }

        void onMouseMove(const Point<int>& pt, bool isHovered) override {
            m_isHovered = isHovered;
            if (!isHovered) m_isPressed = false;
        }

    private:
        std::string m_text;
        std::function<void()> m_onClick;
        bool m_isHovered = false;
        bool m_isPressed = false;
    };

    class Checkbox : public Widget {
    public:
        GUI_WIDGET_MOVABLE(Checkbox)

        Checkbox(const std::string& text) : m_text(text) {}

        Checkbox& onToggle(std::function<void(bool)> callback) & noexcept 
        {
            m_onToggle = std::move(callback);
            return *this;
        }

        Checkbox&& onToggle(std::function<void(bool)> callback) && noexcept 
        {
            m_onToggle = std::move(callback);
            return std::move(*this);
        }
        void onPaint(Renderer::Renderer& r) override {
            r.drawRect(m_bounds, m_isHovered ? Color::fromRgb(235, 235, 235) : Color::fromRgb(255, 255, 255));
            
            std::string visualText = m_checked ? "[X] " : "[  ] ";
            visualText += m_text;
            Renderer::TextPosition pos = {
                m_bounds,
                Renderer::TextAlignment::Center
            };
            r.drawText(visualText, pos, Color::fromRgb(0, 0, 0));
        }

        bool onMouseButtonDown(const Point<int>& pt) override {
            return true;
        }

        bool onMouseButtonUp(const Point<int>& pt) override {
            if (m_isHovered) {
                m_checked = !m_checked;
                if (m_onToggle) m_onToggle(m_checked);
            }
            return true;
        }

        void onMouseMove(const Point<int>& pt, bool isHovered) override {
            m_isHovered = isHovered;
        }

    private:
        std::string m_text;
        bool m_checked = false;
        bool m_isHovered = false;
        std::function<void(bool)> m_onToggle;
    };
}