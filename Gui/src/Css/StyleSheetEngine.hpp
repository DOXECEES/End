#pragma once

#include "StyleSheet.hpp"
#include <Padding.hpp>

#include <string>

namespace gui
{

    class StyleSheetEngine
    {
      public:
        template <typename T>
        static T getProperty(const std::string& name, const Widget& widget, T defaultValue = T(0)) noexcept
        {
            return styleSheet.getProperty<T>(name, widget, defaultValue);
        }

        template <typename T>
        static T getProperty(const std::string& name, const Window& window, T defaultValue = T(0)) noexcept
        {
            return styleSheet.getProperty<T>(name, window, defaultValue);
        }

        static void setStyleSheet(const StyleSheet& sheet) noexcept
        {
            styleSheet = sheet;
        }

      private:
        inline static StyleSheet styleSheet;
    };

    template <>
    inline Padding
    StyleSheetEngine::getProperty<Padding>(const std::string& name, const Widget& widget, Padding defaultValue) noexcept
    {
        Padding base = styleSheet.getProperty<Padding>(name, widget, defaultValue);

        return Padding { .left  = styleSheet.getProperty<int>(name + "-left", widget, base.left),
                         .top   = styleSheet.getProperty<int>(name + "-top", widget, base.top),
                         .right = styleSheet.getProperty<int>(name + "-right", widget, base.right),
                         .bottom = styleSheet.getProperty<int>(name + "-bottom", widget, base.bottom) };
    }

}; // namespace gui