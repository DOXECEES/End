#include "LayoutManager.hpp"

#include <Flags.hpp>

#include "Application.hpp"
#include "Cursor.hpp"
#include "DockNode.hpp"
#include "Window/Window.hpp"

#include <algorithm>

namespace gui
{
    void LayoutManager::recalcLayout(Window* parent) noexcept
    {
        if (!rootNode)
        {
            return;
        }

        Rect<int> rect = parent->getClientRect();

        const int width  = rect.width;
        const int height = rect.height - TOOLBAR_HEIGHT;
        if (width <= 0 || height <= 0)
        {
            return;
        }

        const Rect<int> clientRect = { 0, TOOLBAR_HEIGHT, width, height };

        Application* app = Application::instance();

        positioner.beginBatch(app->getCountOfWindows());
        layoutNode(rootNode, clientRect);
        positioner.endBatch();

        parent->repaint(true);
    }

    void LayoutManager::addChild(Window* parent) noexcept
    {
        DockNode* leaf = findFirstLeaf(rootNode);
        if (!leaf)
        {
            return;
        }

        const std::string title = "Окно " + std::to_string(windowCounter++);

        Window* wnd = new Window(0, 0, title, parent);

        if (wnd)
        {
            leaf->windows.push_back(wnd);
            leaf->activeIndex = static_cast<int>(leaf->windows.size()) - 1;

            auto* data = reinterpret_cast<ChildData*>(
                wnd->getWindowPointerData(WindowDataPointerType::UserData)
            );
            if (data)
            {
                data->dockNode = leaf;
            }

            recalcLayout(parent);
        }
    }

    void LayoutManager::removeChild(Window* parent) noexcept
    {
        DockNode* leaf = findFirstLeaf(rootNode);
        if (!leaf || leaf->windows.empty())
        {
           
            return;
        }

        Window* destroyWindow = leaf->windows.back();
        
        removeWindowFromNode(leaf, destroyWindow);
        delete destroyWindow;

        recalcLayout(parent);
    }

    void
    LayoutManager::removeWindowFromNode(DockNode* leaf, Window* child) noexcept
    {
        if (!leaf)
        {
            return;
        }

        auto& vec = leaf->windows;
        for (auto it = vec.begin(); it != vec.end(); ++it)
        {
            if (*it == child)
            {
                vec.erase(it);
                break;
            }
        }

        if (leaf->activeIndex >= static_cast<int>(vec.size()))
        {
            leaf->activeIndex = static_cast<int>(vec.size()) - 1;
        }
        if (leaf->activeIndex < 0)
        {
            leaf->activeIndex = 0;
        }

        if (vec.empty())
        {
            DockNode* parent = leaf->parent;
            if (parent)
            {
                DockNode* sibling =
                    (parent->child1 == leaf) ? parent->child2 : parent->child1;
                DockNode* grandParent = parent->parent;

                sibling->parent = grandParent;
                if (grandParent)
                {
                    if (grandParent->child1 == parent)
                    {
                        grandParent->child1 = sibling;
                    }
                    else
                    {
                        grandParent->child2 = sibling;
                    }
                }
                else
                {
                    rootNode = sibling;
                }

                delete leaf;
                delete parent;
                
            }
        }
    }

    void LayoutManager::detachWindow(Window* child, Window* parent) noexcept
    {
        auto* data = reinterpret_cast<ChildData*>(
            child->getWindowPointerData(WindowDataPointerType::UserData)
        );
        if (!data || data->isFloating)
        {
            return;
        }

        data->isFloating = true;

        removeWindowFromNode(data->dockNode, child);
        data->dockNode = nullptr;

        constexpr auto styles = WindowStyles::PopUp | WindowStyles::Caption |
                                WindowStyles::ThickFrame |
                                WindowStyles::SystemMenu |
                                WindowStyles::Visible;

        child->setStyleBatch(styles);
        child->setParent(nullptr);

        const Point<int> cursorPos = Cursor::getPosition();

        positioner.beginBatch(1);

        positioner.setGeometry(
            child,
            Rect<int> { cursorPos.x - 150, cursorPos.y - 15, 300, 200 },
            PositionerFlag::FrameChanged | PositionerFlag::ShowWindow
        );

        positioner.endBatch();

        recalcLayout(parent);

       
        recalcLayout(parent);

        child->startSystemMove(cursorPos);
    }

    void LayoutManager::splitLeafNode(
        DockNode* leaf, Window* child, DockAction action, Window* parent
    ) noexcept
    {
        if (!leaf || !child)
        {
            return;
        }

        auto* childData = reinterpret_cast<ChildData*>(
            child->getWindowPointerData(WindowDataPointerType::UserData)
        );
        if (!childData)
        {
            return;
        }

        childData->isFloating = false;

        if (action == DockAction::Tab)
        {
            leaf->windows.push_back(child);
            leaf->activeIndex   = static_cast<int>(leaf->windows.size()) - 1;
            childData->dockNode = leaf;
        }
        else
        {
            DockNode* c1 = new (std::nothrow) DockNode();
            DockNode* c2 = new (std::nothrow) DockNode();
            if (!c1 || !c2)
            {
                delete c1;
                delete c2;
                
                return;
            }

            c1->type        = NodeType::Leaf;
            c1->parent      = leaf;
            c1->windows     = leaf->windows;
            c1->activeIndex = leaf->activeIndex;

            for (Window* hw : c1->windows)
            {
                auto* d = reinterpret_cast<ChildData*>(
                    hw->getWindowPointerData(WindowDataPointerType::UserData)
                );
                if (d)
                {
                    d->dockNode = c1;
                }
            }

            c2->type   = NodeType::Leaf;
            c2->parent = leaf;
            c2->windows.push_back(child);
            c2->activeIndex     = 0;
            childData->dockNode = c2;

            leaf->windows.clear();
            leaf->activeIndex = 0;

            if (action == DockAction::Left)
            {
                leaf->type       = NodeType::SplitHorizontal;
                leaf->child1     = c2;
                leaf->child2     = c1;
                leaf->splitRatio = 0.3;
               
            }
            else if (action == DockAction::Right)
            {
                leaf->type       = NodeType::SplitHorizontal;
                leaf->child1     = c1;
                leaf->child2     = c2;
                leaf->splitRatio = 0.7;
               
            }
            else if (action == DockAction::Top)
            {
                leaf->type       = NodeType::SplitVertical;
                leaf->child1     = c2;
                leaf->child2     = c1;
                leaf->splitRatio = 0.3;
                
            }
            else if (action == DockAction::Bottom)
            {
                leaf->type       = NodeType::SplitVertical;
                leaf->child1     = c1;
                leaf->child2     = c2;
                leaf->splitRatio = 0.7;
                
            }
        }

        constexpr auto styles = WindowStyles::Child | WindowStyles::Visible |
                                WindowStyles::ClipSiblings;
        child->setStyleBatch(styles);

        child->setParent(parent);

        positioner.beginBatch(1);
        positioner.setGeometry(
            child,
            Rect<int>::zero(),
            PositionerFlag::NoMove | PositionerFlag::NoSize |
                PositionerFlag::NoZOrder | PositionerFlag::FrameChanged
        );
        positioner.endBatch();

        recalcLayout(parent);
    }

    void LayoutManager::layoutNode(DockNode* node, Rect<int> r) noexcept
    {
        if (!node)
        {
            return;
        }
        node->rect = r;

        if (node->type == NodeType::Leaf)
        {
            if (node->windows.empty())
            {
                return;
            }

            const int numWindows = static_cast<int>(node->windows.size());
            node->activeIndex =
                std::clamp(node->activeIndex, 0, numWindows - 1);

            if (node->activeIndex != node->lastActiveIndex)
            {
                for (int i = 0; i < numWindows; ++i)
                {
                    if (i != node->activeIndex)
                    {
                        node->windows[i]->hide();
                    }
                }
                node->windows[node->activeIndex]->show();
                node->lastActiveIndex = node->activeIndex;
            }

            Window* active = node->windows[node->activeIndex];

            r.width  = std::max(0, r.width);
            r.height = std::max(0, r.height);

            positioner.setGeometry(
                active, r, PositionerFlag::NoZOrder | PositionerFlag::NoActivate
            );

            auto* data = reinterpret_cast<ChildData*>(
                active->getWindowPointerData(WindowDataPointerType::UserData)
            );
            if (data)
            {
                data->dockNode = node;
            }
        }
        else

        {
            if (node->type == NodeType::SplitHorizontal)
            {
                const int splitX =
                    r.x + static_cast<int>(r.width * node->splitRatio);

                const int leftWidth  = (splitX - SPLITTER_GAP / 2) - r.x;
                const int rightX     = splitX + SPLITTER_GAP / 2;
                const int rightWidth = (r.x + r.width) - rightX;

                layoutNode(
                    node->child1, Rect<int> { r.x, r.y, leftWidth, r.height }
                );
                layoutNode(
                    node->child2,
                    Rect<int> { rightX, r.y, rightWidth, r.height }
                );
            }
            else 
            {
                const int splitY =
                    r.y + static_cast<int>(r.height * node->splitRatio);

                const int topHeight    = (splitY - SPLITTER_GAP / 2) - r.y;
                const int bottomY      = splitY + SPLITTER_GAP / 2;
                const int bottomHeight = (r.y + r.height) - bottomY;

                layoutNode(
                    node->child1, Rect<int> { r.x, r.y, r.width, topHeight }
                );
                layoutNode(
                    node->child2,
                    Rect<int> { r.x, bottomY, r.width, bottomHeight }
                );
            }
        }
    }
}; // namespace gui