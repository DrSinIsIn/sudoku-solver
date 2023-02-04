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

        for (std::size_t i = 0; i < Grid::columnCount; ++i)
        {
            found |= solveHiddenTuplesFor(gridDescriptor, Column{ gridDescriptor.columnMask(i), i, Grid::maxValue });
            found |= solveHiddenTuplesFor(gridDescriptor, Row{ gridDescriptor.rowMask(i), i, Grid::maxValue });
            found |= solveHiddenTuplesFor(gridDescriptor, Box{ gridDescriptor.boxMask(i), i, Grid::maxValue });
        }

        return found;
    }

private:
    struct House
    {
        House(Bitset const& mask, std::size_t index, std::size_t size)
            : mask{ mask }
            , houseIndex{ index }
            , houseSize{ size }
        {}

        Bitset mask;
        std::size_t houseIndex{};
        std::size_t houseSize{};

        virtual std::size_t getCellAbsoluteIndex(std::size_t localIndex) const = 0;
    };

    struct Row : House
    {
        using House::House;

        std::size_t getCellAbsoluteIndex(std::size_t localIndex) const override
        {
            return Grid::coordinatesToCell(localIndex, House::houseIndex);
        }
    };

    struct Column : House
    {
        using House::House;

        std::size_t getCellAbsoluteIndex(std::size_t localIndex) const override
        {
            return Grid::coordinatesToCell(House::houseIndex, localIndex);
        }
    };

    struct Box : House
    {
        using House::House;

        std::size_t getCellAbsoluteIndex(std::size_t localIndex) const override
        {
            auto const [boxX, boxY] = Grid::cellToCoordinates(Grid::boxIndexToTopLeftCell(House::houseIndex));
            return Grid::coordinatesToCell(boxX + (localIndex % Grid::boxWidth), boxY + (localIndex / Grid::boxWidth));
        }
    };

    std::array<Integer, tupleSize> m_tupleValuesBuffer{};

    template<std::size_t recursionIndex = 0>
    bool solveHiddenTuplesFor(GridDescriptor& descriptor
                            , House const& house
                            , Bitset const& cellsMask = {}
                            , std::size_t startIndex = 0
                            , std::integral_constant<std::size_t, recursionIndex> = {})
    {
        bool hasSolved = false;
        if constexpr (recursionIndex == tupleSize)
        {
            return solveHiddenTuplesFor(descriptor, house.mask, cellsMask);
        }
        else
        {
            std::size_t const maxCellIndex = house.houseSize - (tupleSize - recursionIndex);
            for (; startIndex <= maxCellIndex;)
            {
                Bitset const newCellsMask = cellsMask | descriptor.cellMask(house.getCellAbsoluteIndex(startIndex));
                hasSolved |= solveHiddenTuplesFor(descriptor
                                                , house
                                                , newCellsMask
                                                , ++startIndex
                                                , std::integral_constant<std::size_t, recursionIndex + 1> {});
            }
        }

        return hasSolved;
    }

    template<std::size_t recursionIndex = 0>
    bool solveHiddenTuplesFor(GridDescriptor& descriptor
                            , Bitset const& houseMask
                            , Bitset const& cellsMask
                            , Integer startValue = 0
                            , std::integral_constant<std::size_t, recursionIndex> = {})
    {
        bool hasSolved = false;
        if constexpr (recursionIndex == tupleSize)
        {
            auto const oldPossibilities = descriptor.possibilities();
            auto const hiddenTupleMask = findHiddenTupleFor(descriptor
                                                          , houseMask
                                                          , cellsMask
                                                          , std::make_index_sequence<tupleSize> {});

            descriptor.possibilities() &= hiddenTupleMask;
            hasSolved = oldPossibilities != descriptor.possibilities();
        }
        else
        {
            std::size_t const maxValue = Grid::maxValue - (tupleSize - recursionIndex);
            for (; startValue < maxValue;)
            {
                m_tupleValuesBuffer[recursionIndex] = ++startValue;
                hasSolved |= solveHiddenTuplesFor(descriptor
                                                , houseMask
                                                , cellsMask
                                                , startValue
                                                , std::integral_constant<std::size_t, recursionIndex + 1> {});
            }
        }

        return hasSolved;
    }

    template<std::size_t... indices>
    Bitset findHiddenTupleFor(GridDescriptor const& descriptor
                            , Bitset const& houseMask
                            , Bitset const& cellsMask
                            , std::index_sequence<indices...>) const
    {
        auto const valuesPossibilities = (descriptor.possibilitiesForValue(m_tupleValuesBuffer[indices]) | ...);
        auto const valuesHouseMask = valuesPossibilities & houseMask;
        auto const hiddenTupleMask = valuesPossibilities & cellsMask;

        if (hiddenTupleMask == valuesHouseMask)
        {
            return ~cellsMask | hiddenTupleMask;
        }

        return Bitset{}.set();
    }
};

template<typename Grid>
using HiddenSingleSolver = HiddenTupleSolver<1, Grid>;
