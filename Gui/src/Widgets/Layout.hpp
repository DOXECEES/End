// Layout.hpp
#pragma once
#include "Widget.hpp"
#include <memory>
#include <utility>
#include <vector>

namespace gui
{

    class Layout : public Widget
    {
      public:
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
            m_children.push_back(std::move(child));
            onBoundsChanged();
        }

        void onAttached() override
        {
            for (auto& child : m_children)
            {
                child->setParent(this);
            }
        }

        void onDetached() override
        {
            for (auto& child : m_children)
            {
                child->setParent(nullptr);
            }
        }

        const std::vector<std::unique_ptr<Widget>>& getChildren() const noexcept
        {
            return m_children;
        }

        void onPaint(Renderer::Renderer& r) override
        {
            for (auto& child : m_children)
            {
                child->onPaint(r);
            }
        }

        bool onMouseButtonDown(const Point<int>& pt) override
        {
            for (auto& child : m_children)
            {
                if (child->getBounds().contains(pt))
                {
                    m_activeChild = child.get();
                    return child->onMouseButtonDown(pt);
                }
            }
            return false;
        }

        bool onMouseButtonUp(const Point<int>& pt) override
        {
            if (m_activeChild)
            {
                bool handled  = m_activeChild->onMouseButtonUp(pt);
                m_activeChild = nullptr;
                return handled;
            }
            return false;
        }

        void onMouseMove(const Point<int>& pt, bool isHovered) override
        {
            Widget* hoveredChild = nullptr;
            if (isHovered)
            {
                for (auto& child : m_children)
                {
                    if (child->getBounds().contains(pt))
                    {
                        hoveredChild = child.get();
                        break;
                    }
                }
            }

            for (auto& child : m_children)
            {
                child->onMouseMove(pt, child.get() == hoveredChild);
            }
        }

      protected:
        std::vector<std::unique_ptr<Widget>> m_children;
        Widget* m_activeChild = nullptr;
    };
} // namespace gui