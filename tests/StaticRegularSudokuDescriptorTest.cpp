// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "Sudoku.h"
#include "SudokuDescriptor.h"

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
        ASSERT_FALSE(descriptor.missingValues().test(cellIndex30));

        // Its value is 1
        auto const cellMask30 = descriptor.cellMask(cellIndex30);
        ASSERT_FALSE((descriptor.possibleCellsFor(1) & cellMask30).none());

        // Its value couldn't be anything other than 1
        for (unsigned value{ 2 }; value <= 9; ++value)
        {
            ASSERT_TRUE((descriptor.possibleCellsFor(value) & cellMask30).none());
        }
    }

    {
        // Cell at (0, 0) is missing its value
        auto const cellIndex00 = ::subjectGrid.coordinatesToCell(0, 0);
        ASSERT_TRUE(descriptor.missingValues().test(cellIndex00));

        // Its value couldn't be 1
        auto const cellMask00 = descriptor.cellMask(cellIndex00);
        ASSERT_TRUE((descriptor.possibleCellsFor(1) & cellMask00).none());

        // Its value could be 3
        ASSERT_FALSE((descriptor.possibleCellsFor(3) & cellMask00).none());
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
    ASSERT_TRUE(descriptor.missingValues().test(cellIndex70));

    auto const cellMask70 = descriptor.cellMask(cellIndex70);
    auto const uniqueValueIt = std::ranges::find_if(descriptor.possibleCellsPerValue()
                                                    , [&](auto const& bitset)
                                                    {
                                                        return !(bitset & cellMask70).none();
                                                    });

    // ...where only possible value is 9
    ASSERT_EQ(std::addressof(*uniqueValueIt), std::addressof(descriptor.possibleCellsFor(9)));

    // Tell descriptor that this value is not missing anymore
    descriptor.missingValues().reset(cellIndex70);

    SRSudoku9x9 const resultGrid = descriptor;
    // New value should have been set in the resulting grid
    ASSERT_EQ(*(resultGrid.begin() + cellIndex70), 9);
}
