// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <version>

namespace details
{
    template<std::uintmax_t n, std::unsigned_integral UInt0, std::unsigned_integral... UInts>
    struct FirstAccomodating
        : std::conditional_t<std::in_range<UInt0>(n), std::type_identity<UInt0>, FirstAccomodating<n, UInts...>>
    {
    };

    template<std::uintmax_t n, std::unsigned_integral UInt>
    struct FirstAccomodating<n, UInt> : std::enable_if<std::in_range<UInt>(n), UInt>
    {
    };

} // namespace details

template<std::unsigned_integral Integer_, std::size_t boxWidth_, std::size_t boxHeight_>
    requires ((boxWidth_ * boxHeight_) > 0)
           && (std::in_range<Integer_>(boxWidth_ * boxHeight_))
class StaticRegularSudoku
{
public:
    static constexpr std::size_t boxWidth = boxWidth_;
    static constexpr std::size_t boxHeight = boxHeight_;
    static constexpr std::size_t maxValue = (boxWidth * boxHeight);
    static constexpr std::size_t rowCount = maxValue;
    static constexpr std::size_t columnCount = maxValue;
    static constexpr std::size_t boxCount = maxValue;
    static constexpr std::size_t cellCount = (maxValue * maxValue);

    static constexpr std::size_t coordinatesToCell(std::size_t x, std::size_t y) noexcept
    {
        return x + (y * columnCount);
    }

    static constexpr std::size_t cellToX(std::size_t i) noexcept
    {
        return i % columnCount;
    }

    static constexpr std::size_t cellToY(std::size_t i) noexcept
    {
        return i / columnCount;
    }

    static constexpr std::pair<std::size_t, std::size_t> cellToCoordinates(std::size_t i) noexcept
    {
        return { cellToX(i), cellToY(i) };
    }

    static constexpr std::size_t cellToBoxTopLeftCell(std::size_t i) noexcept
    {
        auto const [x, y] = cellToCoordinates(i);
        return coordinatesToCell((x - (x % boxWidth)), (y - (y % boxHeight)));
    }

    static constexpr std::size_t cellToBoxIndex(std::size_t i) noexcept
    {
        auto const [x, y] = cellToCoordinates(i);
        return (x / boxWidth) + (y - (y % boxHeight));
    }

    static constexpr std::size_t boxIndexToTopLeftCell(std::size_t i) noexcept
    {
        return coordinatesToCell((i % boxHeight) * boxWidth, i - (i % boxHeight));
    }

    using Integer = Integer_;
    using Array = std::array<Integer, cellCount>;

    StaticRegularSudoku() = default;

    template<std::convertible_to<Integer>... Ints>
        requires (sizeof...(Ints) == cellCount)
    explicit constexpr StaticRegularSudoku(Ints&&... ints) noexcept
        : m_array{ static_cast<Integer>(ints)... }
    {}

    constexpr auto begin() const noexcept { return m_array.begin(); }
    constexpr auto end() const noexcept { return m_array.end(); }
    constexpr auto begin() noexcept { return m_array.begin(); }
    constexpr auto end() noexcept { return m_array.end(); }

    constexpr bool isFilled() const noexcept
    {
        return std::ranges::all_of(m_array, std::identity{});
    }

#if (__cpp_lib_constexpr_bitset >= 202207L)
    constexpr
#endif
    bool isValid() const noexcept
    {
        std::array<std::bitset<maxValue>, columnCount> columnChecks{};
        std::array<std::bitset<maxValue>, rowCount> rowChecks{};
        std::array<std::bitset<maxValue>, boxCount> boxChecks{};

        for (std::size_t i = 0; i < cellCount; ++i)
        {
            Integer const val = m_array[i];
            if (val > maxValue)
            {
                return false;
            }

            if (val == 0)
            {
                continue;
            }

            auto const [x, y] = cellToCoordinates(i);
            std::array<std::reference_wrapper<std::bitset<maxValue>>, 3> const toCheck = {
                columnChecks[x], rowChecks[y], boxChecks[cellToBoxIndex(i)]
            };

            for (auto& bitset : toCheck)
            {
                if (bitset.get().test(val - 1))
                {
                    return false;
                }

                bitset.get().set(val - 1);
            }
        }

        return true;
    }

#if (__cpp_lib_constexpr_bitset >= 202207L)
    constexpr
#endif
    bool isSolved() const noexcept
    {
        return isFilled() && isValid();
    }

private:
    Array m_array{};
};

template<std::size_t boxWidth, std::size_t boxHeight>
using Sudoku = StaticRegularSudoku<
    typename details::FirstAccomodating<boxWidth * boxHeight
                                      , std::uint8_t
                                      , std::uint16_t
                                      , std::uint32_t
                                      , std::uint64_t>::type
  , boxWidth
  , boxHeight>;

using Sudoku9 = Sudoku<3, 3>; // Classic
using Sudoku4 = Sudoku<2, 2>;
