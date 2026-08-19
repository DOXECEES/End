#pragma once

#include <Color.hpp>
#include <WideStringUtils.hpp>

#include "Cursor.hpp"
#include "LayoutManager.hpp"
#include "Window/Window.hpp"

#include <algorithm>
#include <string>

namespace gui
{
    constexpr int HEADER_HEIGHT_THRESHOLD = 25;
    constexpr int MAX_TAB_WIDTH           = 150;
    constexpr int DRAG_THRESHOLD_SQR      = 100;
    class DockWorkspace : public Window
    {
      public:
        DockWorkspace(int width, int height, const std::string& title) : Window(width, height, title, nullptr)
        {
            highlightWindow = std::make_unique<Window>(0, 0, "", nullptr);
            highlightWindow->hide();
            highlightWindow->setStyleBatch(WindowStyles::PopUp);
            highlightWindow->addTransparency(100);
            highlightWindow->addTransparencyForInput();
            highlightWindow->setTopMost(true);
            highlightWindow->skipTaskbar();
            
            highlightWindow->onPaint([this](Renderer::Renderer& r)
            {
                Rect<int> rect = highlightWindow->getClientRect();

                r.clear(Color::fromRgb(180, 210, 255));

                const int thickness = 4;
                r.drawRect({ 0, 0, rect.width, thickness }, Color::fromRgb(0, 100, 220));
                r.drawRect({ 0, rect.height - thickness, rect.width, thickness }, Color::fromRgb(0, 100, 220));
                r.drawRect({ 0, 0, thickness, rect.height }, Color::fromRgb(0, 100, 220));
                r.drawRect({ rect.width - thickness, 0, thickness, rect.height }, Color::fromRgb(0, 100, 220));
            });

            auto* root = new (std::nothrow) DockNode();
            root->type = NodeType::Leaf;
            layoutManager.setRoot(root);

            onPaint([this](Renderer::Renderer& r)
            {
                r.clear(Color::fromRgb(170, 170, 170));
            });

            onSize([this](int w, int h)
            {
                layoutManager.recalcLayout(this);
            });

            onMouseButtonDown([this](const Point<int>& pos)
            {
                auto hit = layoutManager.hitTestSplitters(layoutManager.getRoot(), pos.x, pos.y);
                if (hit.node)
                {
                    activeSplitNode = hit.node;
                    setCapture();
                }
            });

            onMouseButtonUp([this](const Point<int>& pos)
            {
                if (activeSplitNode)
                {
                    activeSplitNode = nullptr;
                    releaseCapture();
                }
            });

            onMouseMove([this](const Point<int>& pos)
            {
                if (!activeSplitNode)
                {
                    return;
                }

                Rect<int> r = activeSplitNode->rect;

                if (activeSplitNode->type == NodeType::SplitHorizontal)
                {
                    double ratio                = static_cast<double>(pos.x - r.x) / r.width;
                    activeSplitNode->splitRatio = std::clamp(ratio, 0.1, 0.9);
                }
                else
                {
                    double ratio                = static_cast<double>(pos.y - r.y) / r.height;
                    activeSplitNode->splitRatio = std::clamp(ratio, 0.1, 0.9);
                }

                layoutManager.recalcLayout(this);
            });
        }

        void dock(Window* window, DockAction action) noexcept
        {
            window->setStyleBatch(WindowStyles::Child | WindowStyles::Visible | WindowStyles::ClipSiblings);
            window->setParent(this);

            auto* data = new (std::nothrow) ChildData();
            window->setWindowPointerData(WindowDataPointerType::UserData, data);

            DockNode* targetLeaf = layoutManager.findFirstLeaf(layoutManager.getRoot());
            if (targetLeaf)
            {
                layoutManager.splitLeafNode(targetLeaf, window, action, this);
            }

            window->onMouseButtonDown([this, window, data](const Point<int>& pos)
            {
                if (data->isFloating || !data->dockNode || pos.y >= HEADER_HEIGHT_THRESHOLD)
                {
                    return;
                }

                DockNode* leaf    = data->dockNode;
                const auto& vec   = leaf->windows;
                const size_t size = vec.size();

                if (size > 1)
                {
                    Rect<int> rect = window->getClientRect();
                    int tabWidth   = rect.width / static_cast<int>(size);
                    if (tabWidth > MAX_TAB_WIDTH)
                    {
                        tabWidth = MAX_TAB_WIDTH;
                    }
                    if (tabWidth <= 0)
                    {
                        tabWidth = 1;
                    }

                    const int clickedIndex = pos.x / tabWidth;
                    if (clickedIndex >= 0 && clickedIndex < static_cast<int>(size))
                    {
                        if (vec[clickedIndex] == window)
                        {
                            data->isDraggingHeader = true;
                            data->dragStart        = pos;
                        }
                        else
                        {
                            leaf->activeIndex = clickedIndex;
                            layoutManager.recalcLayout(this);
                        }
                    }
                }
                else
                {
                    data->isDraggingHeader = true;
                    data->dragStart        = pos;
                }
            });

            window->onMouseMove([this, window, data](const Point<int>& pos)
            {
                if (data->isDraggingHeader)
                {
                    const int dx = pos.x - data->dragStart.x;
                    const int dy = pos.y - data->dragStart.y;

                    if ((dx * dx + dy * dy) > DRAG_THRESHOLD_SQR)
                    {
                        data->isDraggingHeader = false;

                        layoutManager.detachWindow(window, this);
                        window->startSystemMove(pos);
                    }
                }
            });

            window->onMouseButtonUp([data](const Point<int>& pos)
            {
                data->isDraggingHeader = false;
            });

            window->onMoving([this, window]()
            {
                handleFloatingWindowDrag(window);
            });

            window->onExitSizeMove([this, window, data]()
            {
                if (data->isFloating && hoveredLeaf && hoveredAction != DockAction::None)
                {
                    layoutManager.splitLeafNode(hoveredLeaf, window, hoveredAction, this);
                }

                hoveredLeaf   = nullptr;
                hoveredAction = DockAction::None;
                highlightWindow->hide();
            });

            window->onPaint([this, window](Renderer::Renderer& r)
            {
                drawChildWindow(window, r);
            });

            window->repaint(true);
        }

      private:
        void handleFloatingWindowDrag(Window* draggedWindow) noexcept
        {
            Point<int> pt = mapFromScreen(Cursor::getPosition());
            hoveredLeaf   = layoutManager.findLeafNodeAtPoint(layoutManager.getRoot(), pt.x, pt.y);

            if (hoveredLeaf)
            {
                const auto preview = layoutManager.getDockPreview(hoveredLeaf, pt.x, pt.y);
                hoveredAction      = preview.action;

                Rect<int> r          = preview.rect;
                Point<int> screenPos = mapToScreen({ r.x, r.y });

                highlightWindow->setGeometry({ screenPos.x, screenPos.y, r.width, r.height });
                highlightWindow->show();
            }
            else
            {
                hoveredAction = DockAction::None;
                highlightWindow->hide();
            }
        }

        void drawChildWindow(Window* child, Renderer::Renderer& r) noexcept
        {
            auto* data = reinterpret_cast<ChildData*>(child->getWindowPointerData(WindowDataPointerType::UserData));
            if (!data)
            {
                return;
            }

            const bool floating = data->isFloating;
            DockNode* leaf      = data->dockNode;
            Rect<int> rect      = child->getClientRect();

            r.clear(Color::fromRgb(245, 245, 245));

            int contentTop = 0;
            if (!floating && leaf)
            {
                contentTop      = 25;
                const auto& vec = leaf->windows;
                const size_t N  = vec.size();

                r.drawRect({ rect.x, rect.y, rect.width, 25 }, Color::fromRgb(170, 170, 170));

                if (N > 1)
                {
                    int tabWidth = rect.width / static_cast<int>(N);
                    if (tabWidth > MAX_TAB_WIDTH)
                    {
                        tabWidth = MAX_TAB_WIDTH;
                    }

                    for (size_t i = 0; i < N; ++i)
                    {
                        Rect<int> tabRect      = { rect.x + static_cast<int>(i) * tabWidth, rect.y, tabWidth, 25 };
                        const bool isActiveTab = (vec[i] == child);

                        Color tabColor  = isActiveTab ? Colors::White : Color::fromRgb(200, 200, 200);
                        Color textColor = isActiveTab ? Colors::Black : Color::fromRgb(100, 100, 100);

                        r.drawRect({ tabRect.x, tabRect.y, tabRect.width - 1, tabRect.height }, tabColor);

                        std::string tabTitle = vec[i]->getTitle();

                        Renderer::TextPosition pos = {
                            { tabRect.x + 8, tabRect.y, tabRect.width - 16, tabRect.height },
                            Renderer::TextAlignment::Center
                        };

                        r.drawText(tabTitle, pos, textColor);
                    }
                }
                else
                {
                    r.drawRect({ rect.x, rect.y, rect.width, 25 }, Color::fromRgb(220, 220, 220));

                    std::string title = child->getTitle();

                    Renderer::TextPosition pos = {
                        { rect.x + 10, rect.y, rect.width - 20, 25 },
                        Renderer::TextAlignment::Left
                    };

                    r.drawText(title, pos, Color::fromRgb(50, 50, 50));

                    Renderer::TextPosition pos2 = {
                        { rect.x, rect.y, rect.width - 10, 25 },
                        Renderer::TextAlignment::Right
                    };

                    r.drawText("[Потяните шапку для отстыковки]", pos2, Color::fromRgb(100, 100, 100));
                }
            }

            Rect<int> contentRect = { rect.x, rect.y + contentTop, rect.width, rect.height - contentTop };


            std::string title = child->getTitle();

            std::string displayMsg = title + (floating ? "\n(Плавающий режим)" : "\n(Пристыкован)");
            if (floating)
            {
                displayMsg += "\n\nПотяните это окно и наведите на любой край "
                              "другого\nприкрепленного окна, чтобы разделить "
                              "его пополам.";
            }

            Renderer::TextPosition pos = { contentRect, Renderer::TextAlignment::Left };

            r.drawText(displayMsg, pos, Colors::Black);
        }

      private:
        LayoutManager layoutManager;
        std::unique_ptr<Window> highlightWindow;

        DockNode* hoveredLeaf     = nullptr;
        DockAction hoveredAction  = DockAction::None;
        DockNode* activeSplitNode = nullptr;
    };
} // namespace gui