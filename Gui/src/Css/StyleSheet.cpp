#include "StyleSheet.hpp"

#include "Widgets/Widget.hpp"

namespace gui
{

    void StyleSheet::addRule(const std::string& name, const Rules& rules)
    {
        Selector sel;
        std::string s = name;
        
        size_t colon = s.find(':');
        if (colon != std::string::npos)
        {
            sel.state = s.substr(colon + 1);
            s = s.substr(0, colon);
        }
        sel.type = s;

        registry[sel] = rules;

    }
    
    std::string StyleSheet::widgetStateToString(const Widget& widget) noexcept 
    {
        switch (widget.getState())
        {
            case WidgetState::Normal:
                return "normal";
            case WidgetState::Hovered:
                return "hover";
            case WidgetState::Pressed:
                return "pressed";
            case WidgetState::Disabled:
                return "disabled";
            default:
                return "normal";
        }
    }
    std::optional<std::string> StyleSheet::findProperty(const Selector& selector, const std::string& name)
    {
        auto it = registry.find(selector);

        if (it != registry.end())
        {
            auto ruleIt = it->second.find(name);
            if (ruleIt != it->second.end())
            {
                return ruleIt->second;
            }
        }
        return std::nullopt;
    }

}; // namespace gui