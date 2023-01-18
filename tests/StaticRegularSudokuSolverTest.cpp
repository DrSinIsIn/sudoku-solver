// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "Solvers/NakedSingleSolver.h"
#include "Solvers/Utility/SudokuDescriptor.h"
#include "Sudoku.h"

#include <algorithm>
#include <array>

namespace
{
    using SRSudoku9x9 = StaticRegularSudoku<unsigned, 3, 3>;

    inline constexpr SRSudoku9x9 pureNakedSingleSolvable{ 0, 0, 0, 1, 0, 5, 0, 0, 0, //
                                                          1, 4, 0, 0, 0, 0, 6, 7, 0, //
                                                          0, 8, 0, 0, 0, 2, 4, 0, 0, //
                                                          0, 6, 3, 0, 7, 0, 0, 1, 0, //
                                                          9, 0, 0, 0, 0, 0, 0, 0, 3, //
                                                          0, 1, 0, 0, 9, 0, 5, 2, 0, //
                                                          0, 0, 7, 2, 0, 0, 0, 8, 0, //
                                                          0, 2, 6, 0, 0, 0, 0, 3, 5, //
                                                          0, 0, 0, 4, 0, 9, 0, 0, 0 };
}

TEST(StaticRegularSudokuSolverTest, nakedSingleSolver_solveOnce)
{
    NakedSingleSolver<SRSudoku9x9> solver;

    SudokuDescriptor<SRSudoku9x9> const startDescriptor{ ::pureNakedSingleSolvable };
    SudokuDescriptor<SRSudoku9x9> descriptor{ startDescriptor };

    // One or more naked singles have been found
    ASSERT_TRUE(solver.solveOnce(descriptor));

    // Exact count should be 2
    auto const newValues = startDescriptor.missingValues() & ~descriptor.missingValues();
    ASSERT_EQ(newValues.count(), 2);

    // Cell (7, 0) should have been found & solved
    auto const cellIndex70 = ::pureNakedSingleSolvable.coordinatesToCell(7, 0);
    ASSERT_FALSE(descriptor.missingValues().test(cellIndex70));

    // Cell (7, 8) should have been found & solved
    auto const cellIndex78 = ::pureNakedSingleSolvable.coordinatesToCell(7, 8);
    ASSERT_FALSE(descriptor.missingValues().test(cellIndex78));

    // Converting back
    SRSudoku9x9 const resultGrid = descriptor;
    ASSERT_TRUE(resultGrid.isValid());
    // Value at (7, 0) should now be 9
    ASSERT_EQ(*(resultGrid.begin() + cellIndex70), 9);
    // Value at (7, 8) should now be 6
    ASSERT_EQ(*(resultGrid.begin() + cellIndex78), 6);

    auto const [mismatchIt, _] = std::ranges::mismatch(resultGrid
                                                     , ::pureNakedSingleSolvable
                                                     , [](auto value, auto model)
                                                       {
                                                           return (model == 0) || (value == model);
                                                       }
    );

    // Nothing has been changed that shouldn't have been
    ASSERT_EQ(mismatchIt, resultGrid.end());

    {
        SRSudoku9x9 gridToSolve = ::pureNakedSingleSolvable;

        ASSERT_TRUE(solver(gridToSolve));

        // Same result as with the descriptor
        auto const [mismatchIt, _] = std::ranges::mismatch(gridToSolve, resultGrid);
        ASSERT_EQ(mismatchIt, gridToSolve.end());
    }
}

TEST(StaticRegularSudokuSolverTest, solvePureNakedSingleSolvable)
{
    NakedSingleSolver<SRSudoku9x9> solver;
    SRSudoku9x9 gridToSolve = ::pureNakedSingleSolvable;

    std::size_t oldSolvedCount = std::ranges::count_if(gridToSolve, [](auto i) { return i > 0; });
    while (solver(gridToSolve))
    {
        ASSERT_TRUE(gridToSolve.isValid());
        std::size_t newSolvedCount = std::ranges::count_if(gridToSolve, [](auto i) { return i > 0; });
        ASSERT_GT(newSolvedCount, oldSolvedCount);
        oldSolvedCount = newSolvedCount;
    }

    ASSERT_TRUE(gridToSolve.isSolved()); // All solved

    auto const [mismatchIt, _] = std::ranges::mismatch(gridToSolve
                                                     , ::pureNakedSingleSolvable
                                                     , [](auto value, auto model)
                                                       {
                                                           return (model == 0) || (value == model);
                                                       }
    );

    // Nothing has been changed that shouldn't have been
    ASSERT_EQ(mismatchIt, gridToSolve.end());
}
