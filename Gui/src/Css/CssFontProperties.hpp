#pragma once

#include <string>

namespace gui
{
    struct CssFontProperties
    {
        std::string family;
        float size;
        int weight;
        bool italic;

        std::string getCacheKey() const
        {
            return family + "_" + std::to_string(size) + "_" + std::to_string(weight) + "_" + (italic ? "I" : "N");
        }
    };
}; // namespace gui
