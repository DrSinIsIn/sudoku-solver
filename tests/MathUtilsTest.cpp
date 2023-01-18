// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "Solvers/Utility/MathUtils.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <set>

namespace
{
    inline constexpr std::array<std::size_t, 1> firstSubsetOf1{ 0 };
    inline constexpr std::array<std::size_t, 3> firstSubsetOf3{ 0, 1, 2 };
}

TEST(MathUtilsTest, nChooseN)
{
    auto firstCombination = firstSubsetOf3;

    ASSERT_FALSE(MathUtils::nextCombination<3>(firstCombination));

    ASSERT_EQ(firstCombination, firstSubsetOf3);
}

TEST(MathUtilsTest, nChooseR)
{
    auto current = firstSubsetOf3;

    using Combination = decltype(current);
    std::set<Combination> results;

    do
    {
        // make sure there is no duplicate
        std::set<std::size_t> numbers{ current.begin(), current.end() };
        ASSERT_EQ(numbers.size(), 3);

        ASSERT_TRUE(results.insert(current).second);
    }
    while (MathUtils::nextCombination<5>(current));

    ASSERT_EQ(current, firstSubsetOf3);
    ASSERT_EQ(results.size(), 10);
}
