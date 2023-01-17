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
    using SRSudoku9x9 = StaticRegularSudoku<unsigned, 3, 3>;

    inline constexpr SRSudoku9x9 emptyGrid{ 0, 0, 0, 0, 0, 0, 0, 0, 0, //
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, //
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, //
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, //
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, //
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, //
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, //
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, //
                                            0, 0, 0, 0, 0, 0, 0, 0, 0 };

    inline constexpr SRSudoku9x9 startingGrid{ 0, 0, 0, 1, 0, 5, 0, 0, 0, //
                                               1, 4, 0, 0, 0, 0, 6, 7, 0, //
                                               0, 8, 0, 0, 0, 2, 4, 0, 0, //
                                               0, 6, 3, 0, 7, 0, 0, 1, 0, //
                                               9, 0, 0, 0, 0, 0, 0, 0, 3, //
                                               0, 1, 0, 0, 9, 0, 5, 2, 0, //
                                               0, 0, 7, 2, 0, 0, 0, 8, 0, //
                                               0, 2, 6, 0, 0, 0, 0, 3, 5, //
                                               0, 0, 0, 4, 0, 9, 0, 0, 0 };

    inline constexpr SRSudoku9x9 solvedGrid{ 6, 7, 2, 1, 4, 5, 3, 9, 8, //
                                             1, 4, 5, 9, 8, 3, 6, 7, 2, //
                                             3, 8, 9, 7, 6, 2, 4, 5, 1, //
                                             2, 6, 3, 5, 7, 4, 8, 1, 9, //
                                             9, 5, 8, 6, 2, 1, 7, 4, 3, //
                                             7, 1, 4, 3, 9, 8, 5, 2, 6, //
                                             5, 9, 7, 2, 3, 6, 1, 8, 4, //
                                             4, 2, 6, 8, 1, 7, 9, 3, 5, //
                                             8, 3, 1, 4, 5, 9, 2, 6, 7 };

    inline constexpr SRSudoku9x9 filledGridWithDuplicate{ 6, 7, 2, 1, 4, 5, 3, 9, 8, //
                                                          1, 4, 5, 9, 8, 3, 6, 7, 2, //
                                                          3, 8, 9, 7, 6, 2, 4, 5, 1, //
                                                          2, 6, 3, 5, 7, 4, 8, 1, 9, //
                                                          9, 5, 8, 6, 2, 1, 7, 4, 3, //
                                                          7, 1, 4, 3, 9, 8, 5, 2, 6, //
                                                          5, 9, 7, 2, 3, 8, 1, 8, 4, //
                                                          4, 2, 6, 8, 1, 7, 9, 3, 5, //
                                                          8, 3, 1, 4, 5, 9, 2, 6, 7 };

    inline constexpr SRSudoku9x9 filledGridWithOverMax{ 6, 7, 2,  1, 4, 5, 3, 9, 8, //
                                                        1, 4, 5, 10, 8, 3, 6, 7, 2, //
                                                        3, 8, 9,  7, 6, 2, 4, 5, 1, //
                                                        2, 6, 3,  5, 7, 4, 8, 1, 9, //
                                                        9, 5, 8,  6, 2, 1, 7, 4, 3, //
                                                        7, 1, 4,  3, 9, 8, 5, 2, 6, //
                                                        5, 9, 7,  2, 3, 6, 1, 8, 4, //
                                                        4, 2, 6,  8, 1, 7, 9, 3, 5, //
                                                        8, 3, 1,  4, 5, 9, 2, 6, 7 };
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

TEST(StaticRegularSudokuTest, isFilled)
{
    ASSERT_FALSE(emptyGrid.isFilled());
    ASSERT_FALSE(startingGrid.isFilled());
    ASSERT_TRUE(solvedGrid.isFilled());
    ASSERT_TRUE(filledGridWithDuplicate.isFilled());
    ASSERT_TRUE(filledGridWithOverMax.isFilled());
}

TEST(StaticRegularSudokuTest, isValid)
{
    ASSERT_TRUE(emptyGrid.isValid());
    ASSERT_TRUE(startingGrid.isValid());
    ASSERT_TRUE(solvedGrid.isValid());
    ASSERT_FALSE(filledGridWithDuplicate.isValid());
    ASSERT_FALSE(filledGridWithOverMax.isValid());
}
