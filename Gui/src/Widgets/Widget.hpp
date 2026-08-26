#pragma once
#include "Renderer/Renderer.hpp"
#include <Types.hpp>


#include <memory>
#include <string_view>

#define GUI_WIDGET_MOVABLE(ClassName)                                                                                  \
    std::unique_ptr<gui::Widget> moveToHeap() && override                                                              \
    {                                                                                                                  \
        return std::make_unique<ClassName>(std::move(*this));                                                          \
    }

#define REGISTER_WIDGET_CLASS_NAME(ClassName) \
    std::string_view getClassName() const override { return #ClassName; }

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

    enum class WidgetState
    {
        Normal,
        Hovered,
        Pressed,
        Disabled,
        Focused
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
            return parent;
        }
        [[nodiscard]] Window* getWindow() const noexcept
        {
            return window;
        }

        void setParent(Widget* localParent) noexcept
        {
            parent = localParent;
            if (parent)
            {
                window = parent->getWindow();
                onAttached();
            }
            else
            {
                window = nullptr;
                onDetached();
            }
        }

        void setRootWindow(Window* localWindow) noexcept
        {
            window = localWindow;
            onAttached();
        }

        virtual void onAttached() {}
        virtual void onDetached() {}

        void setBounds(const Rect<int>& localBounds) noexcept
        {
            bounds = localBounds;
            onBoundsChanged();
        }
        [[nodiscard]] const Rect<int>& getBounds() const noexcept
        {
            return bounds;
        }

        virtual void onPaint(Renderer::Renderer& r);


        virtual bool onMouseButtonDown(const Point<int>& pt)
        {
            return false;
        }
        virtual bool onMouseButtonUp(const Point<int>& pt)
        {
            return false;
        }
        virtual void onMouseMove(const Point<int>& pt, bool isHovered)
        {
            
        }

        virtual void onCharInput(wchar_t ch) {}

        virtual std::unique_ptr<Widget> moveToHeap() && = 0;

        const WidgetSizePolicy& getVerticalSizePolicy() const noexcept;
        const WidgetSizePolicy& getHorizontalSizePolicy() const noexcept;

        Widget& fixedHeight(int h) & noexcept;
        Widget&& fixedHeight(int h) && noexcept;

        Widget& autoHeight() & noexcept;
        Widget&& autoHeight() && noexcept;

        Widget& stretchHeight(int weight = 1) & noexcept;
        Widget&& stretchHeight(int weight = 1) && noexcept;

        Widget& fixedWidth(int w) & noexcept;
        Widget&& fixedWidth(int w) && noexcept;

        Widget& autoWidth() & noexcept;
        Widget&& autoWidth() && noexcept;

        Widget& stretchWidth(int weight = 1) & noexcept;
        Widget&& stretchWidth(int weight = 1) && noexcept;

        virtual std::string_view getClassName() const = 0;

        void setStyleSheetClass(std::string className) noexcept { styleSheetClass = std::move(className); }
        const std::string& getStyleSheetClass() const { return styleSheetClass; }

        Widget& addClass(std::string_view className) & noexcept { styleSheetClass = className; return *this; }
        Widget&& addClass(std::string_view className) && noexcept { styleSheetClass = className; return std::move(*this); }

        WidgetState getState() const noexcept { return state; }

        bool isFocused() const noexcept;

        virtual int getPreferredHeight() const;

        virtual Widget* getWidgetAt(const Point<int>& pt)
        {
            if (pt.x >= bounds.x && pt.y >= bounds.y && 
                pt.x < bounds.x + bounds.width && pt.y < bounds.y + bounds.height)
            {
                return this;
            }
            return nullptr;
        }


      protected:
        virtual void onBoundsChanged() {}

        Rect<int> bounds { 0, 0, 0, 0 };
        Widget* parent = nullptr;
        Window* window = nullptr;
        WidgetSizePolicy verticalSizePolicy;
        WidgetSizePolicy horizontalSizePolicy;

        std::string styleSheetClass;

        WidgetState state = WidgetState::Normal;
    };
} // namespace gui