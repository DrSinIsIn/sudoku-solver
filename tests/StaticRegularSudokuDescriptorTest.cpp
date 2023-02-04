// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "Sudoku.h"
#include "Solvers/Utility/SudokuDescriptor.h"

#include <algorithm>
#include <array>

namespace
{
    using SRSudoku9x9 = StaticRegularSudoku<unsigned, 3, 3>;

    inline constexpr SRSudoku9x9 subjectGrid{ 0, 0, 0, 1, 0, 5, 0, 0, 0, //
                                              1, 4, 0, 0, 0, 0, 6, 7, 0, //
                                              0, 8, 0, 0, 0, 2, 4, 0, 0, //
                                              0, 6, 3, 0, 7, 0, 0, 1, 0, //
                                              9, 0, 0, 0, 0, 0, 0, 0, 3, //
                                              0, 1, 0, 0, 9, 0, 5, 2, 0, //
                                              0, 0, 7, 2, 0, 0, 0, 8, 0, //
                                              0, 2, 6, 0, 0, 0, 0, 3, 5, //
                                              0, 0, 0, 4, 0, 9, 0, 0, 0 };
}

TEST(StaticRegularSudokuDescriptorTest, descriptor)
{
    SudokuDescriptor<SRSudoku9x9> const descriptor{ ::subjectGrid };

    {
        // Cell at (0, 3) has a value
        auto const cellIndex30 = ::subjectGrid.coordinatesToCell(3, 0);
        auto const cellMask30 = descriptor.cellMask(cellIndex30);
        auto const missingValueAtCell30Mask = descriptor.missingValuesMask() & cellMask30;
        ASSERT_TRUE(missingValueAtCell30Mask.none());

        // Its value is 1
        auto const value1Mask = descriptor.possibilitiesForValue(1);
        ASSERT_TRUE((value1Mask & cellMask30).any());

        // Its value couldn't be anything other than 1
        auto const otherValuesMask = descriptor.possibilities() ^ value1Mask;
        ASSERT_TRUE((otherValuesMask & cellMask30).none());
    }

    {
        // Cell at (0, 0) is missing its value
        auto const cellIndex00 = ::subjectGrid.coordinatesToCell(0, 0);
        auto const cellMask00 = descriptor.cellMask(cellIndex00);
        auto const missingValueAtCell00Mask = descriptor.missingValuesMask() & cellMask00;
        ASSERT_TRUE(missingValueAtCell00Mask == cellMask00);

        // Its value couldn't be 1
        auto const value1Mask = descriptor.possibilitiesForValue(1);
        ASSERT_TRUE((value1Mask & cellMask00).none());

        // Its value could be 3
        auto const value3Mask = descriptor.possibilitiesForValue(3);
        ASSERT_TRUE((value3Mask & cellMask00).any());
    }

    // Converting back
    SRSudoku9x9 const resultGrid = descriptor;
    auto [mismatchIt, _] = std::ranges::mismatch(resultGrid, ::subjectGrid);

    // Nothing has been changed
    ASSERT_EQ(mismatchIt, resultGrid.end());
}

TEST(StaticRegularSudokuDescriptorTest, modifyingDescriptor)
{
    SudokuDescriptor<SRSudoku9x9> descriptor{ ::subjectGrid };

    // Cell at (7, 0) is a naked single...
    auto const cellIndex70 = ::subjectGrid.coordinatesToCell(7, 0);
    auto const cellMask70 = descriptor.cellMask(cellIndex70);
    auto const missingValueAtCell70Mask = descriptor.missingValuesMask() & cellMask70;
    ASSERT_TRUE(missingValueAtCell70Mask == cellMask70);

    // ...where only possible value is 9
    auto const value9Mask = descriptor.possibilitiesForValue(9);
    ASSERT_TRUE((value9Mask & cellMask70).any());

    auto const otherValuesMask = descriptor.possibilities() ^ value9Mask;
    ASSERT_TRUE((otherValuesMask & cellMask70).none());

    // Tell descriptor that this value is not missing anymore
    descriptor.missingValuesMask() &= ~cellMask70;

    SRSudoku9x9 const resultGrid = descriptor;
    // New value should have been set in the resulting grid
    ASSERT_EQ(*(resultGrid.begin() + cellIndex70), 9);
}
