// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>

namespace MathUtils
{
    template<std::size_t setSize, std::unsigned_integral Element, std::size_t subsetSize>
        requires (subsetSize <= setSize)
    constexpr bool nextCombination(std::array<Element, subsetSize>& inOutCombination)
    {
        std::array<bool, setSize> permutable{};
        for (auto index : inOutCombination)
        {
            permutable[index] = true;
        }

        bool const isNewCombinationLexicographicallyNext
            = std::prev_permutation(permutable.begin(), permutable.end());

        auto outIt = inOutCombination.begin();
        for (Element element{}; outIt != inOutCombination.end(); ++element)
        {
            if (permutable[element])
            {
                *(outIt++) = element;
            }
        }

        return isNewCombinationLexicographicallyNext;
    }
}
