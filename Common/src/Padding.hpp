#pragma once

#include "Types.hpp"

template <typename T>
struct BoxOffsets
{
    T left      = T(0);
    T top       = T(0);
    T right     = T(0);
    T bottom    = T(0);
};

using Padding = BoxOffsets<int32>;
using Margin  = BoxOffsets<int32>;

