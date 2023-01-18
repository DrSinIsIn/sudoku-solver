// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <numeric>
#include <array>
#include <cstddef>

#include "AbstractSolver.h"
#include "Utility/MathUtils.h"

template<std::size_t size, typename Grid>
    requires (size > 0) && (size < Grid::columnCount / 2)
class BasicFishSolver : public AbstractSolver<Grid>
{
public:
    using GridDescriptor = typename AbstractSolver<Grid>::GridDescriptor;
    using Bitset = typename AbstractSolver<Grid>::Bitset;
    using Integer = typename AbstractSolver<Grid>::Integer;

    bool solveOnce(GridDescriptor& gridDescriptor) override
    {
        using Indices = std::array<std::size_t, size>;

        constexpr Indices firstIndexCombination = []
        {
            Indices result;
            std::iota(result.begin(), result.end(), 0);
            return result;
        }();

        Indices rowIndices = firstIndexCombination;
        Indices colIndices = firstIndexCombination;

        bool found = false;
        do
        {
            Bitset rowsMask;

            for (auto i : rowIndices)
            {
                rowsMask |= gridDescriptor.rowMask(i);
            }

            do
            {
                Bitset colsMask;

                for (auto i : colIndices)
                {
                    colsMask |= gridDescriptor.columnMask(i);
                }

                found |= solveBasicFish(gridDescriptor, rowsMask, colsMask);

            } while (MathUtils::nextCombination<Grid::maxValue>(colIndices));
        } while (MathUtils::nextCombination<Grid::maxValue>(rowIndices));

        return found;
    }

private:
    bool solveBasicFish(GridDescriptor& gridDescriptor, Bitset const& rowsMask, Bitset const& colsMask) const
    {
        bool found = false;

        for (auto& possibleCells : gridDescriptor.possibleCellsPerValue())
        {
            Bitset const inRows = possibleCells & rowsMask;
            Bitset const inCols = possibleCells & colsMask;
            Bitset const possibleFish = inRows & inCols;
            if (possibleFish.none() || (possibleFish == (inRows | inCols)))
            {
                continue;
            }

            if (inRows == possibleFish)
            {
                possibleCells &= ~(inCols ^ possibleFish);
                found = true;
            }
            else if (inCols == possibleFish)
            {
                possibleCells &= ~(inRows ^ possibleFish);
                found = true;
            }
        }

        return found;
    }
};

template<typename Grid>
using XWingSolver = BasicFishSolver<2, Grid>;
