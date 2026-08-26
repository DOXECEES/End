// Layout.hpp
#pragma once
#include "Widget.hpp"
#include "Css/StyleSheetEngine.hpp"


#include <memory>
#include <utility>
#include <vector>

namespace gui
{

    class Layout : public Widget
    {
      public:
        REGISTER_WIDGET_CLASS_NAME(Layout)
        GUI_WIDGET_MOVABLE(Layout)
        Layout() = default;

        template <typename... Args> Layout(Args&&... args)
        {
            addChildren(std::forward<Args>(args)...);
        }

        template <typename... Args> void addChildren(Args&&... args)
        {
            (addChild(std::forward<Args>(args).moveToHeap()), ...);
        }

        virtual ~Layout() = default;

        Layout(const Layout&)            = delete;
        Layout& operator=(const Layout&) = delete;

        Layout(Layout&&) noexcept            = default;
        Layout& operator=(Layout&&) noexcept = default;

        virtual void addChild(std::unique_ptr<Widget> child)
        {
            if (!child)
            {
                return;
            }
            child->setParent(this);
            children.push_back(std::move(child));
            onBoundsChanged();
        }

        void onAttached() override
        {
            for (auto& child : children)
            {
                child->setParent(this);
            }
        }

        void onDetached() override
        {
            for (auto& child : children)
            {
                child->setParent(nullptr);
            }
        }

        const std::vector<std::unique_ptr<Widget>>& getChildren() const noexcept
        {
            return children;
        }

        void onPaint(Renderer::Renderer& r) override
        {
            // Color color = StyleSheetEngine::getProperty<Color>("background-color", *this);

            // int borderRadius = StyleSheetEngine::getProperty<int>("border-radius", *this);

            // if (borderRadius > 0)
            // {
            //     r.drawRoundedRect(m_bounds, color, borderRadius);
            // }
            // else
            // {
            //     r.drawRect(m_bounds, color);
            // }
        
            for (auto& child : children)
            {
                child->onPaint(r);
            }
        }

        bool onMouseButtonDown(const Point<int>& pt) override
        {
            for (auto& child : children)
            {
                if (child->getBounds().contains(pt))
                {
                    activeChild = child.get();
                    return child->onMouseButtonDown(pt);
                }
            }
            return false;
        }

        bool onMouseButtonUp(const Point<int>& pt) override
        {
            if (activeChild)
            {
                bool handled  = activeChild->onMouseButtonUp(pt);
                activeChild = nullptr;
                return handled;
            }
            return false;
        }

        void onMouseMove(const Point<int>& pt, bool isHovered) override
        {
            Widget* hoveredChild = nullptr;
            if (isHovered)
            {
                for (auto& child : children)
                {
                    if (child->getBounds().contains(pt))
                    {
                        hoveredChild = child.get();
                        break;
                    }
                }
            }

            for (auto& child : children)
            {
                child->onMouseMove(pt, child.get() == hoveredChild);
            }
        }

      protected:
        std::vector<std::unique_ptr<Widget>> children;
        Widget* activeChild = nullptr;
    };
} // namespace gui