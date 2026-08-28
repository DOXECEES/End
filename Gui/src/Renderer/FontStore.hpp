#pragma once

#include "GraphicsFactory.hpp"
#include "Css/CssFontProperties.hpp"
#include "Renderer/Font.hpp"

#include <string>
#include <memory>
#include <unordered_map>

namespace Renderer
{
    inline FontWeight toFontWeightFromCss(const int weight)
    {
        switch (weight)
        {
        case 400:
            return FontWeight::Normal;
        case 700:
            return FontWeight::Bold;
        default:
            return FontWeight::Normal;
        }
    }

    class FontStore
    {
      public:
        explicit FontStore(gui::GraphicsFactory& factory) : factory(factory) {}

        std::shared_ptr<Font> getFont(const gui::CssFontProperties& props)
        {
            std::string key = props.getCacheKey();

            auto it = cache.find(key);
            if (it != cache.end())
            {
                return it->second; 
            }

            auto font = factory.createFont(props.family, props.size, props.weight);

            cache[key] = std::move(font);
            return cache[key];
        }

        void collectGarbage()
        {
            for (auto it = cache.begin(); it != cache.end();)
            {
                if (it->second.use_count() == 1)
                { 
                    it = cache.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

    private:
        gui::GraphicsFactory& factory;

        std::unordered_map<std::string, std::shared_ptr<Font>> cache;

    };

} // namespace Renderer