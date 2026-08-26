#pragma once

#include <Color.hpp>
#include <Padding.hpp>

#include "Window/Window.hpp"
#include "Widgets/Widget.hpp"
#include "Cursor.hpp"

#include <algorithm>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <initializer_list>


namespace gui
{
    class Widget;

    using Rules      = std::unordered_map<std::string, std::string>;

    class Selector
    {
      public:
        std::string type;
        std::string state;

        bool operator==(const Selector& other) const noexcept
        {
            return type == other.type && state == other.state;
        }
    };
} // namespace gui

namespace std
{
    template <> struct hash<gui::Selector>
    {
        std::size_t operator()(const gui::Selector& s) const noexcept
        {
            std::size_t h1 = std::hash<std::string> {}(s.type);
            std::size_t h2 = std::hash<std::string> {}(s.state);
            return h1 ^ (h2 << 1);
        }
    };
} // namespace std

namespace gui
{
    class Rule
    {
      public:
        Selector selector;
        Rules rules;
    };

    template <typename T> inline T parseValue(const std::string& str);

    template <> inline CursorType parseValue<CursorType>(const std::string& str)
    {
        if(str == "arrow")
        {
            return CursorType::Arrow;
        }
        if(str == "pointer")
        {
            return CursorType::Hand;
        }
        return CursorType::Arrow;
    }

    template <> inline int parseValue<int>(const std::string& str)
    {
        auto pos = str.find_first_not_of("0123456789");
        return pos == std::string::npos ? std::stoi(str) : std::stoi(str.substr(0, pos));
    }

    template <> inline std::string parseValue<std::string>(const std::string& str)
    {
        return str;
    }

    template <>
    inline BoxOffsets<int> parseValue<BoxOffsets<int>>(const std::string& str)
    {
        std::vector<int> values;
        std::stringstream ss(str);
        std::string token;

        while (ss >> token)
        {
            auto pos = token.find_first_not_of("-0123456789");
            int val = 0;
            if (!token.empty())
            {
                try
                {
                    val = (pos == std::string::npos) ? std::stoi(token) : std::stoi(token.substr(0, pos));
                }
                catch (...)
                {
                    val = 0;
                }
            }
            values.push_back(val);
        }

        BoxOffsets<int> p;
        if (values.empty()) 
        {
            return p;
        }

        if (values.size() == 1)
        {
            p.top = p.right = p.bottom = p.left = values[0];
        }
        else if (values.size() == 2)
        {
            p.top = p.bottom = values[0];
            p.left = p.right = values[1];
        }
        else if (values.size() == 3)
        {
            p.top = values[0];
            p.left = p.right = values[1];
            p.bottom = values[2];
        }
        else
        {
            p.top    = values[0];
            p.right  = values[1];
            p.bottom = values[2];
            p.left   = values[3];
        }

        return p;
    }

    template <> inline Color parseValue<Color>(const std::string& str)
    {
        if (str.rfind("rgb", 0) == 0)
        {
            int r = 0, g = 0, b = 0;
            if (sscanf_s(str.c_str(), "rgb(%d,%d,%d)", &r, &g, &b) == 3)
            {
                return Color::fromRgb(r, g, b);
            }
        }
        return Color {};
    }

    class StyleSheet
    {
      public:
        StyleSheet() = default;
        virtual ~StyleSheet() = default;

        StyleSheet(std::initializer_list<std::pair<std::string, Rules>> initList) {
            for (const auto& [selectorStr, rules] : initList) {
                addRule(selectorStr, rules);
            }
        }

        void addRule(const std::string& selector, const Rules& rules);

        template <typename T> T getProperty(const std::string& name, const Widget& widget, T defaultValue = T(0)) noexcept
        {
            auto className              = widget.getClassName();
            Selector selector           = { .type = widget.getStyleSheetClass(), .state = widgetStateToString(widget) };
            Selector emptyStateSelector = { .type = widget.getStyleSheetClass(), .state = "" };
            Selector classNameStateSelector      = { .type = className.data(), .state = widgetStateToString(widget) };
            Selector emptyClassNameStateSelector = { .type = className.data(), .state = "" };

            if (auto val = findProperty(selector, name))
            {
                return parseValue<T>(*val);
            }
            if (auto val = findProperty(emptyStateSelector, name))
            {
                return parseValue<T>(*val);
            }
            if (auto val = findProperty(classNameStateSelector, name))
            {
                return parseValue<T>(*val);
            }
            if (auto val = findProperty(emptyClassNameStateSelector, name))
            {
                return parseValue<T>(*val);
            }

            return defaultValue;
        }

        template <typename T> 
        T getProperty(const std::string& name, const Window& window, T defaultValue = T(0)) noexcept
        {
            auto className = window.getClassName();
            
            Selector styleSheetClassSelector = { .type = window.getStyleSheetClass(), .state = "" };
            Selector classNameSelector      = { .type = className.data(), .state = "" };

            if (auto val = findProperty(styleSheetClassSelector, name))
            {
                return parseValue<T>(*val);
            }
            
            if (auto val = findProperty(classNameSelector, name))
            {
                return parseValue<T>(*val);
            }

            return defaultValue;
        }

      private:
        std::string widgetStateToString(const Widget& widget) noexcept;

        std::optional<std::string> findProperty(const Selector& selector, const std::string& name);

        std::unordered_map<Selector, Rules> registry;
    };


    

} // namespace gui