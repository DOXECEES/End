#pragma once

#include <Rect.hpp>
#include <Point.hpp>

#include <vector>

namespace gui
{
    class Window;

    enum class DockAction 
    {
        None,
        Tab,
        Left,
        Right,
        Top,
        Bottom
    };

    enum class NodeType 
    {
        Leaf,
        SplitHorizontal,
        SplitVertical
    };

    struct DockNode 
    {
        NodeType type{ NodeType::Leaf };
        DockNode* parent{ nullptr };
        DockNode* child1{ nullptr }; 
        DockNode* child2{ nullptr }; 
        double splitRatio{ 0.5 };     

        std::vector<Window*> windows; 
        int activeIndex{ 0 };       
        int lastActiveIndex = -1;

        Rect<int> rect{ 0, 0, 0, 0 }; 

        ~DockNode() noexcept = default;
    };

    struct ChildData 
    {
        DockNode* dockNode{ nullptr }; 
        bool isFloating{ false };
        bool isDraggingHeader{ false };
        Point<int> dragStart{ 0, 0 };
    };

};