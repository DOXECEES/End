#pragma once

#include <Rect.hpp>

#include "DockNode.hpp"
#include "Window/WindowPositionerBatch.hpp"

namespace gui
{
    class Window;
    static constexpr int TOOLBAR_HEIGHT = 40;
    static constexpr int SPLITTER_GAP = 6;
    static constexpr int SPLITTER_TOLERANCE = 4;
    
    class LayoutManager
    {
      public:
        LayoutManager() noexcept = default;

        ~LayoutManager() noexcept
        {
            freeTree(rootNode);
        }

        LayoutManager(const LayoutManager&)            = delete;
        LayoutManager& operator=(const LayoutManager&) = delete;

        void recalcLayout(Window* parent) noexcept;

        void addChild(Window* parent) noexcept;


        void removeChild(Window* parent) noexcept;
        
        void removeWindowFromNode(DockNode* leaf, Window* child) noexcept;
        

        void detachWindow(Window* child, Window* parent) noexcept;
        

        void splitLeafNode(
            DockNode* leaf, Window* child, DockAction action, Window* parent
        ) noexcept;
        

        [[nodiscard]] DockNode* getRoot() const noexcept
        {
            return rootNode;
        }
        void setRoot(DockNode* newRoot) noexcept
        {
            rootNode = newRoot;
        }

        [[nodiscard]] DockNode* findFirstLeaf(DockNode* node) const noexcept
        {
            if (!node)
            {
                return nullptr;
            }
            if (node->type == NodeType::Leaf)
            {
                return node;
            }
            DockNode* l1 = findFirstLeaf(node->child1);
            if (l1)
            {
                return l1;
            }
            return findFirstLeaf(node->child2);
        }

        [[nodiscard]] DockNode*
        findLeafNodeAtPoint(DockNode* node, int x, int y) const noexcept
        {
            if (!node)
            {
                return nullptr;
            }

            const Rect<int> r = node->rect;
            
            if (x < r.x || x > (r.x + r.width) || y < r.y || y > (r.y + r.height))
            {
                return nullptr;
            }

            if (node->type == NodeType::Leaf)
            {
                return node;
            }

            DockNode* l1 = findLeafNodeAtPoint(node->child1, x, y);
            if (l1)
            {
                return l1;
            }

            return findLeafNodeAtPoint(node->child2, x, y);
        }

        struct SplitterHit
        {
            DockNode* node { nullptr };
        };

        [[nodiscard]] SplitterHit
        hitTestSplitters(DockNode* node, int x, int y) const noexcept
        {
            if (!node || node->type == NodeType::Leaf)
            {
                return { nullptr };
            }

            const SplitterHit h1 = hitTestSplitters(node->child1, x, y);
            if (h1.node)
            {
                return h1;
            }
            const SplitterHit h2 = hitTestSplitters(node->child2, x, y);
            if (h2.node)
            {
                return h2;
            }

            const Rect<int> r = node->rect; 
            const int w = r.width;
            const int h = r.height;

            if (node->type == NodeType::SplitHorizontal)
            {
                const int splitX = r.x + static_cast<int>(w * node->splitRatio);
                if (x >= splitX - SPLITTER_TOLERANCE &&
                    x <= splitX + SPLITTER_TOLERANCE && 
                    y >= r.y && y <= r.y + h)
                {
                    return { node };
                }
            }
            else
            {
                const int splitY = r.y + static_cast<int>(h * node->splitRatio);
                if (y >= splitY - SPLITTER_TOLERANCE &&
                    y <= splitY + SPLITTER_TOLERANCE && 
                    x >= r.x && x <= r.x + w)
                {
                    return { node };
                }
            }

            return { nullptr };
        }

        struct DockPreview
        {
            DockAction action { DockAction::None };
            Rect<int> rect { 0, 0, 0, 0 };
        };

        [[nodiscard]] DockPreview
        getDockPreview(DockNode* leaf, int cx, int cy) const noexcept
        {
            if (!leaf)
            {
                return { DockAction::None };
            }

            const Rect<int> r = leaf->rect; 
            const int w = r.width;
            const int h = r.height;

            const int rx = cx - r.x;
            const int ry = cy - r.y;

            int borderW = w / 5;
            int borderH = h / 5;
            if (borderW < 30)
            {
                borderW = 30;
            }
            if (borderH < 30)
            {
                borderH = 30;
            }

            if (ry < borderH)
            {
                const Rect<int> preview = { r.x, r.y, w, h / 2 };
                return { DockAction::Top, preview };
            }
            if (ry > h - borderH)
            {
                const Rect<int> preview = { r.x, r.y + h - h / 2, w, h / 2 };
                return { DockAction::Bottom, preview };
            }
            if (rx < borderW)
            {
                const Rect<int> preview = { r.x, r.y, w / 2, h };
                return { DockAction::Left, preview };
            }
            if (rx > w - borderW)
            {
                const Rect<int> preview = { r.x + w - w / 2, r.y, w / 2, h };
                return { DockAction::Right, preview };
            }

            return { DockAction::Tab, r };
        }

      private:
        void freeTree(DockNode* node) noexcept
        {
            if (!node)
            {
                return;
            }
            if (node->type != NodeType::Leaf)
            {
                freeTree(node->child1);
                freeTree(node->child2);
            }
            delete node;
        }

        void layoutNode(DockNode* node, Rect<int> r) noexcept;
        

        DockNode* rootNode { nullptr };
        int windowCounter { 0 };

        WindowPositionerBatch positioner;
    };

}; // namespace gui