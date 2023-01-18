// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <array>
#include <bitset>
#include <cstddef>

#include "SetBitIterator.h"

template<typename Grid>
class SudokuDescriptor
{
public:
    using Integer = Grid::Integer;
    using Bitset = std::bitset<Grid::cellCount>;

    static Bitset cellMask(std::size_t index)
    {
        static Bitset mask{ 1 };
        return mask << index;
    }

    static Bitset columnMask(std::size_t x)
    {
        static Bitset mask = makeColumnMask();
        return mask << Grid::coordinatesToCell(x, 0);
    }

    static Bitset rowMask(std::size_t y)
    {
        static Bitset mask = makeRowMask();
        return mask << Grid::coordinatesToCell(0, y);
    }

    static Bitset boxMask(std::size_t index)
    {
        static Bitset mask = makeBoxMask();
        return mask << Grid::boxIndexToTopLeftCell(index);
    }

    static Bitset cellHousesMask(std::size_t cellIndex)
    {
        return columnMask(Grid::cellToX(cellIndex))
             | rowMask(Grid::cellToY(cellIndex))
             | boxMask(Grid::cellToBoxIndex(cellIndex));
    }

public:
    SudokuDescriptor() = default;

    SudokuDescriptor(Grid const& grid)
    {
        m_missingValues.set();
        for (Bitset& possibleCells : m_possibleCellsPerValue)
        {
            possibleCells.set();
        }

        for (std::size_t i = 0; Integer value : grid)
        {
            if (value > 0)
            {
                for (Bitset& possibleCells : m_possibleCellsPerValue)
                {
                    possibleCells &= ~cellMask(i);
                }
                possibleCellsFor(value) &= ~cellHousesMask(i);
                possibleCellsFor(value) |= cellMask(i);
                m_missingValues.reset(i);
            }
            ++i;
        }
    }

    operator Grid() const
    {
        Grid grid;

        for (Integer value = 1; value <= Grid::maxValue; ++value)
        {
            auto const& possibleCells = possibleCellsFor(value);
            for (auto it = SetBitIterator{ possibleCells & ~m_missingValues }; it != SetBitIterator<Bitset>{}; ++it)
            {
                auto const cell = *it;
                *(grid.begin() + cell) = value;
            }
        }

        return grid;
    }

    Bitset& possibleCellsFor(Integer value)
    {
        return m_possibleCellsPerValue[value - 1];
    }

    Bitset const& possibleCellsFor(Integer value) const
    {
        return m_possibleCellsPerValue[value - 1];
    }

    auto& possibleCellsPerValue()
    {
        return m_possibleCellsPerValue;
    }

    auto const& possibleCellsPerValue() const
    {
        return m_possibleCellsPerValue;
    }

    Bitset& missingValues()
    {
        return m_missingValues;
    }

    Bitset const& missingValues() const
    {
        return m_missingValues;
    }

private:
    Bitset m_missingValues;
    std::array<Bitset, Grid::maxValue> m_possibleCellsPerValue = {};

    static Bitset makeRowMask()
    {
        Bitset bitset{};

        for (std::size_t x = 0; x < Grid::columnCount; ++x)
        {
            bitset.set(Grid::coordinatesToCell(x, 0));
        }

        return bitset;
    }

    static Bitset makeColumnMask()
    {
        Bitset bitset{};

        for (std::size_t y = 0; y < Grid::rowCount; ++y)
        {
            bitset.set(Grid::coordinatesToCell(0, y));
        }

        return bitset;
    }

    static Bitset makeBoxMask()
    {
        Bitset bitset{};

        for (std::size_t x = 0; x < Grid::boxWidth; ++x)
        {
            for (std::size_t y = 0; y < Grid::boxHeight; ++y)
            {
                bitset.set(Grid::coordinatesToCell(x, y));
            }
        }

        return bitset;
    }
};
