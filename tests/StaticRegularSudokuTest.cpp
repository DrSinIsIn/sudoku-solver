// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "Sudoku.h"

#include <algorithm>
#include <array>

namespace
{
    inline constexpr StaticRegularSudoku<unsigned, 3, 3> startingGrid{ 0, 0, 0, 1, 0, 5, 0, 0, 0, //
                                                                       1, 4, 0, 0, 0, 0, 6, 7, 0, //
                                                                       0, 8, 0, 0, 0, 2, 4, 0, 0, //
                                                                       0, 6, 3, 0, 7, 0, 0, 1, 0, //
                                                                       9, 0, 0, 0, 0, 0, 0, 0, 3, //
                                                                       0, 1, 0, 0, 9, 0, 5, 2, 0, //
                                                                       0, 0, 7, 2, 0, 0, 0, 8, 0, //
                                                                       0, 2, 6, 0, 0, 0, 0, 3, 5, //
                                                                       0, 0, 0, 4, 0, 9, 0, 0, 0 };
}

TEST(StaticRegularSudokuTest, ctor)
{
    constexpr std::array<unsigned, 81> testAgainst{ 0, 0, 0, 1, 0, 5, 0, 0, 0, //
                                                    1, 4, 0, 0, 0, 0, 6, 7, 0, //
                                                    0, 8, 0, 0, 0, 2, 4, 0, 0, //
                                                    0, 6, 3, 0, 7, 0, 0, 1, 0, //
                                                    9, 0, 0, 0, 0, 0, 0, 0, 3, //
                                                    0, 1, 0, 0, 9, 0, 5, 2, 0, //
                                                    0, 0, 7, 2, 0, 0, 0, 8, 0, //
                                                    0, 2, 6, 0, 0, 0, 0, 3, 5, //
                                                    0, 0, 0, 4, 0, 9, 0, 0, 0 };

    auto const [sudokuIt, _] = std::ranges::mismatch(startingGrid, testAgainst);
    ASSERT_EQ(sudokuIt, startingGrid.end());
}

}
