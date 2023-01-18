// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

#include "AbstractSolver.h"


template<std::size_t tupleSize, typename Grid>
    requires (tupleSize > 0)
          && (tupleSize < Grid::columnCount)
class HiddenTupleSolver : public AbstractSolver<Grid>
{
public:
    using GridDescriptor = typename AbstractSolver<Grid>::GridDescriptor;
    using Bitset = typename AbstractSolver<Grid>::Bitset;
    using Integer = typename AbstractSolver<Grid>::Integer;

    bool solveOnce(GridDescriptor& gridDescriptor) override
    {
        bool found = false;

        std::array<Integer, tupleSize> tupleValuesBuffer{};
        for (std::size_t i = 0; i < Grid::columnCount; ++i)
        {
            found |= solveHiddenTupleFor(gridDescriptor, gridDescriptor.missingValues() & gridDescriptor.columnMask(i), tupleValuesBuffer);
            found |= solveHiddenTupleFor(gridDescriptor, gridDescriptor.missingValues() & gridDescriptor.rowMask(i), tupleValuesBuffer);
            found |= solveHiddenTupleFor(gridDescriptor, gridDescriptor.missingValues() & gridDescriptor.boxMask(i), tupleValuesBuffer);
        }

        return found;
    }

private:

    template<std::size_t recursionIndex = 0>
    bool solveHiddenTupleFor(GridDescriptor& descriptor, Bitset const& mask,
        std::array<Integer, tupleSize>& tupleValuesBuffer,
        Integer startValue = 0,
        std::integral_constant<std::size_t, recursionIndex> = {})
    {
        bool hasSolved = false;
        if constexpr (recursionIndex == tupleSize)
        {
            Bitset const emptySet;
            Bitset const found = findHiddenTupleFor(descriptor, tupleValuesBuffer, mask, std::make_index_sequence<tupleSize> {});
            for (auto& possibleCells : descriptor.possibleCellsPerValue())
            {
                Bitset const intersection = possibleCells & found;
                bool const isInTuple = (possibleCells & mask) == intersection;

                hasSolved |= intersection.any() && !isInTuple;

                // if the current value is from the found tuple, it must not be shaved
                possibleCells &= ~(isInTuple ? emptySet : found);
            }
        }
        else
        {
            for (; startValue < Grid::maxValue;)
            {
                if ((descriptor.possibleCellsFor(++startValue) & mask).none())
                {
                    continue;
                }

                tupleValuesBuffer[recursionIndex] = startValue;
                hasSolved |= solveHiddenTupleFor(
                    descriptor, mask, tupleValuesBuffer, startValue, std::integral_constant<std::size_t, recursionIndex + 1> {});
            }
        }

        return hasSolved;
    }

    template<std::size_t... indices>
    Bitset findHiddenTupleFor(GridDescriptor const& descriptor, std::array<Integer, tupleSize> const& values,
        Bitset const& mask,
        std::index_sequence<indices...>) const
    {
        auto const hiddenTupleMask = (descriptor.possibleCellsFor(values[indices]) | ...) & mask;
        if (hiddenTupleMask.count() <= tupleSize)
        {
            return hiddenTupleMask;
        }

        return {};
    }
};

template<typename Grid>
using HiddenSingleSolver = HiddenTupleSolver<1, Grid>;
