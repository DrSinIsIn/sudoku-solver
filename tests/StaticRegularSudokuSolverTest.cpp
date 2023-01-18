// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "Solvers/HiddenTupleSolver.h"
#include "Solvers/LockedCandidatesSolver.h"
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

    inline constexpr SRSudoku9x9 hiddenSingleFirstStep  { 1, 4, 0, 0, 9, 0, 0, 0, 0, //
                                                          0, 2, 8, 3, 0, 0, 0, 0, 9, //
                                                          0, 6, 0, 7, 0, 0, 0, 0, 0, //
                                                          0, 9, 5, 4, 2, 0, 0, 0, 0, //
                                                          0, 8, 4, 9, 0, 0, 0, 3, 0, //
                                                          0, 0, 0, 0, 7, 5, 9, 4, 0, //
                                                          8, 0, 0, 0, 0, 7, 0, 9, 0, //
                                                          4, 0, 0, 0, 0, 9, 1, 7, 8, //
                                                          0, 0, 0, 0, 8, 0, 0, 6, 3 };

    inline constexpr SRSudoku9x9 hiddenPairExample  { 0, 0, 9, 0, 3, 2, 0, 0, 0, //
                                                      0, 0, 0, 7, 0, 0, 0, 0, 0, //
                                                      1, 6, 2, 0, 0, 0, 0, 0, 0, //
                                                      0, 1, 0, 0, 2, 0, 5, 6, 0, //
                                                      0, 0, 0, 9, 0, 0, 0, 0, 0, //
                                                      0, 5, 0, 0, 0, 0, 1, 0, 7, //
                                                      0, 0, 0, 0, 0, 0, 4, 0, 3, //
                                                      0, 2, 6, 0, 0, 9, 0, 0, 0, //
                                                      0, 0, 5, 8, 7, 0, 0, 0, 0 };
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
}

TEST(StaticRegularSudokuSolverTest, solvePureNakedSingleSolvable)
{
    NakedSingleSolver<SRSudoku9x9> solver;
    SudokuDescriptor<SRSudoku9x9> descriptor{ ::pureNakedSingleSolvable };

    std::size_t oldMissingCount = descriptor.missingValues().count();
    while (solver.solveOnce(descriptor))
    {
        ASSERT_TRUE(SRSudoku9x9{ descriptor }.isValid());
        std::size_t newMissingCount = descriptor.missingValues().count();
        ASSERT_LT(newMissingCount, oldMissingCount);
        oldMissingCount = newMissingCount;
    }

    SRSudoku9x9 const resultGrid = descriptor;
    ASSERT_TRUE(resultGrid.isSolved()); // All solved

    auto const [mismatchIt, _] = std::ranges::mismatch(resultGrid
                                                     , ::pureNakedSingleSolvable
                                                     , [](auto value, auto model)
                                                       {
                                                           return (model == 0) || (value == model);
                                                       }
    );

    // Nothing has been changed that shouldn't have been
    ASSERT_EQ(mismatchIt, resultGrid.end());
}

TEST(StaticRegularSudokuSolverTest, hiddenSingleSolver_solveOnce)
{
    HiddenSingleSolver<SRSudoku9x9> solver;
    SudokuDescriptor<SRSudoku9x9> const startDescriptor{ ::hiddenSingleFirstStep };
    SudokuDescriptor<SRSudoku9x9> descriptor{ startDescriptor };

    {
        NakedSingleSolver<SRSudoku9x9> nakedSingleSolver;

        // No naked singles to solve
        ASSERT_FALSE(nakedSingleSolver.solveOnce(descriptor));

        SRSudoku9x9 const resultGrid = descriptor;

        // Nothing changed
        auto const [mismatchIt, _] = std::ranges::mismatch(resultGrid, ::hiddenSingleFirstStep);
        ASSERT_EQ(mismatchIt, resultGrid.end());
    }

    {
        // Before running solver, cell (1, 8) has multiple possible values
        auto const cellIndex = ::hiddenSingleFirstStep.coordinatesToCell(1, 8);
        auto const cellMask = descriptor.cellMask(cellIndex);

        auto const count = std::ranges::count_if(descriptor.possibleCellsPerValue()
                                               , [&cellMask](auto const& possibilities)
                                                 {
                                                     auto const masked = possibilities & cellMask;
                                                     return !masked.none();
                                                 }
        );

        ASSERT_GT(count, 1);
    }

    {
        // Before running solver, cell (3, 5) has multiple possible values
        auto const cellIndex = ::hiddenSingleFirstStep.coordinatesToCell(3, 5);
        auto const cellMask = descriptor.cellMask(cellIndex);

        auto const count = std::ranges::count_if(descriptor.possibleCellsPerValue()
                                               , [&cellMask](auto const& possibilities)
                                                 {
                                                     auto const masked = possibilities & cellMask;
                                                     return !masked.none();
                                                 }
        );

        ASSERT_GT(count, 1);
    }


    // One or more hidden singles have been found
    ASSERT_TRUE(solver.solveOnce(descriptor));

    {
        // Cell (1, 8)'s only possibility should be 7 now
        auto const cellIndex = ::hiddenSingleFirstStep.coordinatesToCell(1, 8);
        auto const cellMask = descriptor.cellMask(cellIndex);
        auto const onlyAllowedAddress = std::addressof(descriptor.possibleCellsFor(7));
        for (auto const& possibilities : descriptor.possibleCellsPerValue())
        {
            auto const masked = possibilities & cellMask;
            if (std::addressof(possibilities) == onlyAllowedAddress)
            {
                ASSERT_FALSE(masked.none());
            }
            else
            {
                ASSERT_TRUE(masked.none());
            }
        }
    }

    {
        // Cell (3, 5)'s only possibility should be 8 now
        auto const cellIndex = ::hiddenSingleFirstStep.coordinatesToCell(3, 5);
        auto const cellMask = descriptor.cellMask(cellIndex);
        auto const onlyAllowedAddress = std::addressof(descriptor.possibleCellsFor(8));
        for (auto const& possibilities : descriptor.possibleCellsPerValue())
        {
            auto const masked = possibilities & cellMask;
            if (std::addressof(possibilities) == onlyAllowedAddress)
            {
                ASSERT_FALSE(masked.none());
            }
            else
            {
                ASSERT_TRUE(masked.none());
            }
        }
    }
}

TEST(StaticRegularSudokuSolverTest, solvePureSingleSolvable)
{
    NakedSingleSolver<SRSudoku9x9> nakedSolver;
    HiddenSingleSolver<SRSudoku9x9> hiddenSolver;
    SudokuDescriptor<SRSudoku9x9> descriptor{ ::hiddenSingleFirstStep };

    while (nakedSolver.solveOnce(descriptor) || hiddenSolver.solveOnce(descriptor))
    {
        ASSERT_TRUE(SRSudoku9x9{ descriptor }.isValid());
    }

    SRSudoku9x9 const resultGrid = descriptor;
    ASSERT_TRUE(resultGrid.isSolved()); // All solved

    auto const [mismatchIt, _] = std::ranges::mismatch(resultGrid
                                                     , ::hiddenSingleFirstStep
                                                     , [](auto value, auto model)
                                                       {
                                                           return (model == 0) || (value == model);
                                                       }
    );

    // Nothing has been changed that shouldn't have been
    ASSERT_EQ(mismatchIt, resultGrid.end());
}

TEST(StaticRegularSudokuSolverTest, hiddenPairSolver_solveOnce)
{
    HiddenTupleSolver<2, SRSudoku9x9> solver;
    SudokuDescriptor<SRSudoku9x9> const startDescriptor{ ::hiddenPairExample };
    SudokuDescriptor<SRSudoku9x9> descriptor{ startDescriptor };

    {
        // Before running solver, cell (6, 2) has more than 2 possible values
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(6, 2);
        auto const cellMask = descriptor.cellMask(cellIndex);

        auto const count = std::ranges::count_if(descriptor.possibleCellsPerValue()
                                               , [&cellMask](auto const& possibilities)
                                                 {
                                                     auto const masked = possibilities & cellMask;
                                                     return !masked.none();
                                                 }
        );

        ASSERT_GT(count, 2);
    }

    {
        // Before running solver, cell (7, 2) has more than 2 possible values
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(7, 2);
        auto const cellMask = descriptor.cellMask(cellIndex);

        auto const count = std::ranges::count_if(descriptor.possibleCellsPerValue()
                                               , [&cellMask](auto const& possibilities)
                                                 {
                                                     auto const masked = possibilities & cellMask;
                                                     return !masked.none();
                                                 }
        );

        ASSERT_GT(count, 2);
    }


    // One or more hidden pairs have been found
    ASSERT_TRUE(solver.solveOnce(descriptor));

    {
        // Cell (6, 2)'s only possibilities should be 3 & 7 now
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(6, 2);
        auto const cellMask = descriptor.cellMask(cellIndex);
        auto const onlyAllowedAddresses = { std::addressof(descriptor.possibleCellsFor(3))
                                          , std::addressof(descriptor.possibleCellsFor(7)) };
        for (auto const& possibilities : descriptor.possibleCellsPerValue())
        {
            auto const masked = possibilities & cellMask;
            if (std::ranges::find(onlyAllowedAddresses, std::addressof(possibilities)) != onlyAllowedAddresses.end())
            {
                ASSERT_FALSE(masked.none());
            }
            else
            {
                ASSERT_TRUE(masked.none());
            }
        }
    }

    {
        // Cell (7, 2)'s only possibility should be 2 & 6 now
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(7, 2);
        auto const cellMask = descriptor.cellMask(cellIndex);
        auto const onlyAllowedAddresses = { std::addressof(descriptor.possibleCellsFor(3))
                                          , std::addressof(descriptor.possibleCellsFor(7)) };
        for (auto const& possibilities : descriptor.possibleCellsPerValue())
        {
            auto const masked = possibilities & cellMask;
            if (std::ranges::find(onlyAllowedAddresses, std::addressof(possibilities)) != onlyAllowedAddresses.end())
            {
                ASSERT_FALSE(masked.none());
            }
            else
            {
                ASSERT_TRUE(masked.none());
            }
        }
    }
}

TEST(StaticRegularSudokuSolverTest, lockedCandidatesSolver_solveOnce)
{
    LockedCandidatesSolver<SRSudoku9x9> solver;
    SudokuDescriptor<SRSudoku9x9> const startDescriptor{ ::hiddenPairExample };
    SudokuDescriptor<SRSudoku9x9> descriptor{ startDescriptor };

    {
        // Before running solver, cell (6, 1) can be 3
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(6, 1);
        auto const possibilityAtCell = descriptor.possibleCellsFor(3) & descriptor.cellMask(cellIndex);

        ASSERT_TRUE(possibilityAtCell.any());
    }


    // Locked candidates have been found
    ASSERT_TRUE(solver.solveOnce(descriptor));

    {
        // After running solver, cell (6, 1) cannot be 3
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(6, 1);
        auto const possibilityAtCell = descriptor.possibleCellsFor(3) & descriptor.cellMask(cellIndex);

        ASSERT_TRUE(possibilityAtCell.none());
    }
}
