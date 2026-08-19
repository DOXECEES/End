// Widget.hpp
#pragma once
#include "Renderer/Renderer.hpp"
#include <Types.hpp>

#include <memory>

#define GUI_WIDGET_MOVABLE(ClassName)                                                                                  \
    std::unique_ptr<gui::Widget> moveToHeap() && override                                                              \
    {                                                                                                                  \
        return std::make_unique<ClassName>(std::move(*this));                                                          \
    }

namespace gui
{
    class Window;

    enum class SizePolicy
    {
        Fixed,
        Auto,
        Flex,
    };

    struct WidgetSizePolicy
    {
        SizePolicy policy = SizePolicy::Flex;
        int value         = 1;
    };

    class Widget
    {
      public:
        Widget()          = default;
        virtual ~Widget() = default;

        Widget(const Widget&)            = delete;
        Widget& operator=(const Widget&) = delete;

        Widget(Widget&&) noexcept            = default;
        Widget& operator=(Widget&&) noexcept = default;

        [[nodiscard]] Widget* getParent() const noexcept
        {
            return m_parent;
        }
        [[nodiscard]] Window* getWindow() const noexcept
        {
            return m_window;
        }

        void setParent(Widget* parent) noexcept
        {
            m_parent = parent;
            if (m_parent)
            {
                m_window = m_parent->getWindow();
                onAttached();
            }
            else
            {
                m_window = nullptr;
                onDetached();
            }
        }

        void setRootWindow(Window* window) noexcept
        {
            m_window = window;
            onAttached();
        }

        virtual void onAttached() {}
        virtual void onDetached() {}

        void setBounds(const Rect<int>& bounds) noexcept
        {
            m_bounds = bounds;
            onBoundsChanged();
        }
        [[nodiscard]] const Rect<int>& getBounds() const noexcept
        {
            return m_bounds;
        }

        virtual void onPaint(Renderer::Renderer& r) = 0;

        virtual bool onMouseButtonDown(const Point<int>& pt)
        {
            return false;
        }
        virtual bool onMouseButtonUp(const Point<int>& pt)
        {
            return false;
        }
        virtual void onMouseMove(const Point<int>& pt, bool isHovered) {}

        virtual std::unique_ptr<Widget> moveToHeap() && = 0;

        const WidgetSizePolicy& getVerticalSizePolicy() const noexcept;
        const WidgetSizePolicy& getHorizontalSizePolicy() const noexcept;

        Widget& fixedHeight(int h) & noexcept;
        Widget&& fixedHeight(int h) && noexcept;

        Widget& autoHeight() & noexcept;
        Widget&& autoHeight() && noexcept;

        Widget& stretchHeight(int weight = 1) & noexcept;
        Widget&& stretchHeight(int weight = 1) && noexcept;

        // Ширина (Width Policies)
        Widget& fixedWidth(int w) & noexcept;
        Widget&& fixedWidth(int w) && noexcept;

        Widget& autoWidth() & noexcept;
        Widget&& autoWidth() && noexcept;

        Widget& stretchWidth(int weight = 1) & noexcept;
        Widget&& stretchWidth(int weight = 1) && noexcept;

      protected:
        virtual void onBoundsChanged() {}

        Rect<int> m_bounds { 0, 0, 0, 0 };
        Widget* m_parent = nullptr;
        Window* m_window = nullptr;
        WidgetSizePolicy verticalSizePolicy;
        WidgetSizePolicy horizontalSizePolicy;
    };
} // namespace gui