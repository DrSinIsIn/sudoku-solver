// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <utility>

#include "AbstractSolver.h"
#include "Utility/SetBitIterator.h"


template<typename Grid>
class NakedSingleSolver : public AbstractSolver<Grid>
{
public:
    using GridDescriptor = typename AbstractSolver<Grid>::GridDescriptor;
    using Bitset = typename AbstractSolver<Grid>::Bitset;
    using Integer = typename AbstractSolver<Grid>::Integer;

    bool solveOnce(GridDescriptor& gridDescriptor) override
    {
        Bitset const singles = findNakedSingles(gridDescriptor);
        if (singles.none())
        {
            return false;
        }

        solveNakedSingles(gridDescriptor, singles);

        return true;
    }

private:
    Bitset findNakedSingles(GridDescriptor const& gridDescriptor) const
    {
        Bitset singleCandidateCells, multiCandidateCells;

        for (auto possibleCells : gridDescriptor.possibleCellsPerValue())
        {
            possibleCells &= gridDescriptor.missingValues();

            multiCandidateCells |= singleCandidateCells & possibleCells;
            singleCandidateCells |= possibleCells;
            singleCandidateCells &= ~multiCandidateCells;
        }

        return singleCandidateCells;
    }

    void solveNakedSingles(GridDescriptor& gridDescriptor, Bitset const& nakedSingles) const
    {
        gridDescriptor.missingValues() &= ~nakedSingles;

        for (auto& possibleCells : gridDescriptor.possibleCellsPerValue())
        {
            Bitset const valueNakedSingles = possibleCells & nakedSingles;
            for (auto it = SetBitIterator{ valueNakedSingles }; it != SetBitIterator<Bitset>{}; ++it)
            {
                auto const cell = *it;
                possibleCells &= ~gridDescriptor.cellHousesMask(cell);
            }

            possibleCells |= valueNakedSingles;
        }
    }
};
