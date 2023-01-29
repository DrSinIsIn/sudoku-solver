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
        Bitset nakedSingles{};

        Bitset const missingValuesPossibilities = gridDescriptor.possibilities()
                                                & gridDescriptor.missingValuesMask();

        for (std::size_t cell = 0; cell < Grid::cellCount; ++cell)
        {
            auto const cellMask = gridDescriptor.cellMask(cell);
            auto const possibilities = cellMask & missingValuesPossibilities;
            nakedSingles |= (possibilities.count() == 1) ? possibilities : Bitset{};
        }

        return nakedSingles;
    }

    void solveNakedSingles(GridDescriptor& gridDescriptor, Bitset const& nakedSingles) const
    {
        Bitset impossibilities{};
        Bitset cells{};

        for (auto it = SetBitIterator{ nakedSingles }; it != SetBitIterator<Bitset>{}; ++it)
        {
            auto const cell = *it / Grid::maxValue;
            auto const value = 1 + (*it % Grid::maxValue);
            impossibilities |= gridDescriptor.cellHousesMask(cell) & gridDescriptor.valueMask(value);
            cells |= gridDescriptor.cellMask(cell);
        }

        impossibilities &= ~nakedSingles;

        gridDescriptor.missingValuesMask() &= ~cells;
        gridDescriptor.possibilities() &= ~impossibilities;
    }
};
