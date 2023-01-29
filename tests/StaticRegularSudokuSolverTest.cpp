// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "Solvers/BasicFishSolver.h"
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

    inline constexpr SRSudoku9x9 xWingExample { 0, 0, 5, 4, 0, 0, 6, 0, 2, //
                                                0, 0, 6, 0, 2, 0, 1, 5, 0, //
                                                2, 9, 3, 5, 6, 1, 7, 8, 4, //
                                                0, 5, 2, 3, 0, 4, 8, 0, 0, //
                                                3, 0, 1, 2, 0, 6, 4, 0, 5, //
                                                0, 0, 0, 0, 5, 7, 3, 2, 0, //
                                                0, 3, 0, 0, 4, 2, 5, 6, 0, //
                                                0, 2, 4, 0, 0, 5, 9, 0, 0, //
                                                5, 0, 7, 0, 0, 9, 2, 4, 0 };
}

TEST(StaticRegularSudokuSolverTest, nakedSingleSolver_solveOnce)
{
    NakedSingleSolver<SRSudoku9x9> solver;

    SudokuDescriptor<SRSudoku9x9> const startDescriptor{ ::pureNakedSingleSolvable };
    SudokuDescriptor<SRSudoku9x9> descriptor{ startDescriptor };

    // One or more naked singles have been found
    ASSERT_TRUE(solver.solveOnce(descriptor));

    // Exact count should be 2 new solved cells: (7, 0) & (7, 8)
    auto const cellIndex70 = ::pureNakedSingleSolvable.coordinatesToCell(7, 0);
    auto const cellIndex78 = ::pureNakedSingleSolvable.coordinatesToCell(7, 8);
    auto const expectedNewValueCells = descriptor.cellMask(cellIndex70) | descriptor.cellMask(cellIndex78);
    auto const newValueCells = startDescriptor.missingValuesMask() & ~descriptor.missingValuesMask();
    ASSERT_EQ(newValueCells, expectedNewValueCells);

    // Only possibilities should be respectively 9 & 6
    auto const expectedNewValues = (descriptor.cellMask(cellIndex70) & descriptor.valueMask(9))
                                 | (descriptor.cellMask(cellIndex78) & descriptor.valueMask(6));
    auto const newValues = descriptor.possibilities() & newValueCells;
    ASSERT_EQ(newValues, expectedNewValues);

    // Possibilities of respective houses should have been reduced accordingly
    auto const possibilitiesInHousesMask = (descriptor.cellHousesMask(cellIndex70) & descriptor.valueMask(9))
                                         | (descriptor.cellHousesMask(cellIndex78) & descriptor.valueMask(6));
    auto const possibilitiesInHouses = descriptor.possibilities() & possibilitiesInHousesMask;
    ASSERT_EQ(possibilitiesInHouses, expectedNewValues);

    // And nothing else should have changed
    auto const possibilitiesChanged = descriptor.possibilities() ^ startDescriptor.possibilities();
    auto const expectedChangesMask = expectedNewValueCells | possibilitiesInHousesMask;
    auto const unexpectedChanges = possibilitiesChanged & ~expectedChangesMask;
    ASSERT_TRUE(unexpectedChanges.none());

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

    auto oldMissingValues = descriptor.missingValuesMask();
    while (solver.solveOnce(descriptor))
    {
        ASSERT_TRUE(SRSudoku9x9{ descriptor }.isValid());

        // Any new cell solved?
        auto const newMissingValues = descriptor.missingValuesMask();
        ASSERT_TRUE((oldMissingValues & ~newMissingValues).any());

        oldMissingValues = newMissingValues;
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
        auto const cellPossibilitiesMask = descriptor.possibilities() & cellMask;
        ASSERT_GT(cellPossibilitiesMask.count(), 1);

        // And there is only possible place for 7 in column 1
        auto const colMask = descriptor.columnMask(1);
        auto const valuePossibilities = descriptor.possibilitiesForValue(7);
        auto const valuePossibilitiesInCol = colMask & valuePossibilities;
        ASSERT_EQ(valuePossibilitiesInCol.count(), 1);

        // That happens to be cell (1, 8)
        auto const valuePossibilityInCell = cellMask & valuePossibilities;
        ASSERT_EQ(valuePossibilityInCell, valuePossibilitiesInCol);
    }

    {
        // Before running solver, cell (3, 5) has multiple possible values
        auto const cellIndex = ::hiddenSingleFirstStep.coordinatesToCell(3, 5);
        auto const cellMask = descriptor.cellMask(cellIndex);
        auto const cellPossibilitiesMask = descriptor.possibilities() & cellMask;
        ASSERT_GT(cellPossibilitiesMask.count(), 1);

        // And there is only possible place for 8 in row 5
        auto const rowMask = descriptor.rowMask(5);
        auto const valuePossibilities = descriptor.possibilitiesForValue(8);
        auto const valuePossibilitiesInRow = rowMask & valuePossibilities;
        ASSERT_EQ(valuePossibilitiesInRow.count(), 1);

        // That happens to be cell (3, 5)
        auto const valuePossibilityInCell = cellMask & valuePossibilities;
        ASSERT_EQ(valuePossibilityInCell, valuePossibilitiesInRow);
    }

    // One or more hidden singles have been found
    ASSERT_TRUE(solver.solveOnce(descriptor));

    {
        // Cell (1, 8)'s only possibility should be 7 now
        auto const cellIndex = ::hiddenSingleFirstStep.coordinatesToCell(1, 8);
        auto const cellMask = descriptor.cellMask(cellIndex);
        auto const expectedPossibilities = descriptor.valueMask(7) & cellMask;

        auto const actualPossibilities = descriptor.possibilities() & cellMask;
        ASSERT_EQ(actualPossibilities, expectedPossibilities);
    }

    {
        // Cell (3, 5)'s only possibility should be 8 now
        auto const cellIndex = ::hiddenSingleFirstStep.coordinatesToCell(3, 5);
        auto const cellMask = descriptor.cellMask(cellIndex);
        auto const expectedPossibilities = descriptor.valueMask(8) & cellMask;

        auto const actualPossibilities = descriptor.possibilities() & cellMask;
        ASSERT_EQ(actualPossibilities, expectedPossibilities);
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
        auto const cellPossibilitiesMask = descriptor.possibilities() & cellMask;
        ASSERT_GT(cellPossibilitiesMask.count(), 2);
    }

    {
        // Before running solver, cell (7, 2) has more than 2 possible values
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(7, 2);
        auto const cellMask = descriptor.cellMask(cellIndex);
        auto const cellPossibilitiesMask = descriptor.possibilities() & cellMask;
        ASSERT_GT(cellPossibilitiesMask.count(), 2);
    }


    // One or more hidden pairs have been found
    ASSERT_TRUE(solver.solveOnce(descriptor));

    {
        // Cell (6, 2)'s only possibilities should be 3 & 7 now
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(6, 2);
        auto const cellMask = descriptor.cellMask(cellIndex);
        auto const expectedPossibilities = (descriptor.valueMask(3) | descriptor.valueMask(7)) & cellMask;

        auto const actualPossibilities = descriptor.possibilities() & cellMask;
        ASSERT_EQ(actualPossibilities, expectedPossibilities);
    }

    {
        // Cell (7, 2)'s only possibility should be 3 & 7 now
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(7, 2);
        auto const cellMask = descriptor.cellMask(cellIndex);
        auto const expectedPossibilities = (descriptor.valueMask(3) | descriptor.valueMask(7)) & cellMask;

        auto const actualPossibilities = descriptor.possibilities() & cellMask;
        ASSERT_EQ(actualPossibilities, expectedPossibilities);
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
        auto const possibilityAtCell = descriptor.possibilitiesForValue(3) & descriptor.cellMask(cellIndex);

        ASSERT_TRUE(possibilityAtCell.any());
    }


    // Locked candidates have been found
    ASSERT_TRUE(solver.solveOnce(descriptor));

    {
        // After running solver, cell (6, 1) cannot be 3
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(6, 1);
        auto const possibilityAtCell = descriptor.possibilitiesForValue(3) & descriptor.cellMask(cellIndex);

        ASSERT_TRUE(possibilityAtCell.none());
    }
}

TEST(StaticRegularSudokuSolverTest, xWingSolver_solveOnce)
{
    XWingSolver<SRSudoku9x9> solver;
    SudokuDescriptor<SRSudoku9x9> const startDescriptor{ ::xWingExample };
    SudokuDescriptor<SRSudoku9x9> descriptor{ startDescriptor };

    {
        // Before running solver, cell (4, 3) can be 9
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(4, 3);
        auto const possibilityAtCell = descriptor.possibilitiesForValue(9) & descriptor.cellMask(cellIndex);

        ASSERT_TRUE(possibilityAtCell.any());
    }
    {
        // Before running solver, cell (7, 3) can be 9
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(7, 3);
        auto const possibilityAtCell = descriptor.possibilitiesForValue(9) & descriptor.cellMask(cellIndex);

        ASSERT_TRUE(possibilityAtCell.any());
    }


    // We fished an X-Wing
    ASSERT_TRUE(solver.solveOnce(descriptor));

    {
        // After running solver, cell (4, 3) cannot be 9
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(4, 3);
        auto const possibilityAtCell = descriptor.possibilitiesForValue(9) & descriptor.cellMask(cellIndex);

        ASSERT_TRUE(possibilityAtCell.none());
    }
    {
        // After running solver, cell (7, 3) cannot be 9
        auto const cellIndex = ::hiddenPairExample.coordinatesToCell(7, 3);
        auto const possibilityAtCell = descriptor.possibilitiesForValue(9) & descriptor.cellMask(cellIndex);

        ASSERT_TRUE(possibilityAtCell.none());
    }
}
