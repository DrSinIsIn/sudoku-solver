// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bitset>
#include <cstddef>

#include "SetBitIterator.h"

template<typename Grid>
class SudokuDescriptor
{
public:
    using Integer = Grid::Integer;
    using Bitset = std::bitset<Grid::cellCount * Grid::maxValue>;

    static Bitset cellMask(std::size_t index)
    {
        static const Bitset mask = makeCellMask();
        return mask << (index * Grid::maxValue);
    }

    static Bitset columnMask(std::size_t x)
    {
        static const Bitset mask = makeColumnMask();
        return mask << (Grid::coordinatesToCell(x, 0) * Grid::maxValue);
    }

    static Bitset rowMask(std::size_t y)
    {
        static const Bitset mask = makeRowMask();
        return mask << (Grid::coordinatesToCell(0, y) * Grid::maxValue);
    }

    static Bitset boxMask(std::size_t index)
    {
        static const Bitset mask = makeBoxMask();
        return mask << (Grid::boxIndexToTopLeftCell(index) * Grid::maxValue);
    }

    static Bitset cellHousesMask(std::size_t cellIndex)
    {
        return columnMask(Grid::cellToX(cellIndex))
             | rowMask(Grid::cellToY(cellIndex))
             | boxMask(Grid::cellToBoxIndex(cellIndex));
    }

    static Bitset valueMask(Integer value)
    {
        static const Bitset mask = makeValueMask();
        return mask << (value - 1);
    }

public:
    SudokuDescriptor() = default;

    SudokuDescriptor(Grid const& grid)
    {
        m_missingValues.set();
        m_possibilities.set();

        for (std::size_t i = 0; Integer value : grid)
        {
            if (value > 0)
            {
                Bitset const mask = cellMask(i);
                m_missingValues &= ~mask;

                Bitset const gridValueMask = valueMask(value);
                Bitset const restOfHousesMask = cellHousesMask(i) & ~mask;
                m_possibilities &= ~(restOfHousesMask & gridValueMask);
                m_possibilities &= gridValueMask | ~mask;
            }
            ++i;
        }
    }

    operator Grid() const
    {
        Grid grid;

        for (auto it = SetBitIterator{ m_possibilities & ~m_missingValues }; it != SetBitIterator<Bitset>{}; ++it)
        {
            std::size_t const bitIndex = *it;
            auto const cell = bitIndex / Grid::maxValue;
            auto const value = 1 + (bitIndex % Grid::maxValue);
            *(grid.begin() + cell) = value;
        }

        return grid;
    }

    Bitset possibilitiesForValue(Integer value) const
    {
        return m_possibilities & valueMask(value);
    }

    Bitset& possibilities()
    {
        return m_possibilities;
    }

    Bitset const& possibilities() const
    {
        return m_possibilities;
    }

    Bitset& missingValuesMask()
    {
        return m_missingValues;
    }

    Bitset const& missingValuesMask() const
    {
        return m_missingValues;
    }

private:
    Bitset m_missingValues;
    Bitset m_possibilities;

    static Bitset makeFirstCellsMask(std::size_t cellCount)
    {
        Bitset bitset{};

        bitset.set();
        bitset >>= (Grid::cellCount - cellCount) * Grid::maxValue;

        return bitset;
    }

    static Bitset makeRowMask()
    {
        return makeFirstCellsMask(Grid::columnCount);
    }

    static Bitset makeCellMask()
    {
        return makeFirstCellsMask(1);
    }

    static Bitset makeRepeatedPatternMask(Bitset const& pattern, std::size_t repeatSpan, std::size_t repeatCount)
    {
        Bitset bitset{};

        for (std::size_t y = 0; y < repeatCount; ++y)
        {
            bitset <<= repeatSpan;
            bitset |= pattern;
        }

        return bitset;
    }

    static Bitset makeFirstCellsOfEachRowMask(std::size_t cellCount, std::size_t rowCount)
    {
        return makeRepeatedPatternMask(makeFirstCellsMask(cellCount), Grid::columnCount * Grid::maxValue, rowCount);
    }

    static Bitset makeColumnMask()
    {
        return makeFirstCellsOfEachRowMask(1, Grid::rowCount);
    }

    static Bitset makeBoxMask()
    {
        return makeFirstCellsOfEachRowMask(Grid::boxWidth, Grid::boxHeight);
    }

    static Bitset makeValueMask()
    {
        return makeRepeatedPatternMask(Bitset{}.set(0), Grid::maxValue, Grid::cellCount);
    }
};
