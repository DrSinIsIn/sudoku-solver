// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>

#include "AbstractSolver.h"


template<typename Grid>
class LockedCandidatesSolver : public AbstractSolver<Grid>
{
public:
    using GridDescriptor = typename AbstractSolver<Grid>::GridDescriptor;
    using Bitset = typename AbstractSolver<Grid>::Bitset;
    using Integer = typename AbstractSolver<Grid>::Integer;

    bool solveOnce(GridDescriptor& gridDescriptor) override
    {
        bool found = false;
        for (Integer value = 1; value <= Grid::maxValue; ++value)
        {
            Bitset const valueMask = gridDescriptor.valueMask(value);
            for (std::size_t i = 0; i < Grid::boxCount; ++i)
            {
                // reducing possibleCells
                Bitset const boxXValueMask = gridDescriptor.boxMask(i) & valueMask;

                auto const [boxTopLeftCellX, boxTopLeftCellY] =
                    Grid::cellToCoordinates(Grid::boxIndexToTopLeftCell(i));
                for (std::size_t j = 0; j < Grid::boxWidth; ++j)
                {
                    found |= solveLockedCandidatesFor(gridDescriptor.possibilities()
                                                    , boxXValueMask
                                                    , gridDescriptor.columnMask(boxTopLeftCellX + j) & valueMask);
                }

                for (std::size_t j = 0; j < Grid::boxHeight; ++j)
                {
                    found |= solveLockedCandidatesFor(gridDescriptor.possibilities()
                                                    , boxXValueMask
                                                    , gridDescriptor.rowMask(boxTopLeftCellY + j) & valueMask);
                }
            }
        }

        return found;
    }

private:
    bool solveLockedCandidatesFor(Bitset& possibleCells
                                , Bitset const& maskA
                                , Bitset const& maskB) const
    {
        Bitset const possibleCellsXMaskA = possibleCells & maskA;
        Bitset const possibleCellsXMaskB = possibleCells & maskB;

        if (possibleCellsXMaskA == possibleCellsXMaskB)
        {
            return false;
        }

        Bitset const crossPossibilities = possibleCellsXMaskA & possibleCellsXMaskB;
        if (crossPossibilities.none())
        {
            return false;
        }

        if ((possibleCellsXMaskA == crossPossibilities) || (possibleCellsXMaskB == crossPossibilities))
        {
            possibleCells &= ~(maskA ^ maskB);
            return true;
        }

        return false;
    }
};
