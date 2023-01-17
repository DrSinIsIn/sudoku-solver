// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>

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

template<std::unsigned_integral Integer, std::size_t boxWidth_, std::size_t boxHeight_>
    requires ((boxWidth_ * boxHeight_) > 0)
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

    using Array = std::array<Integer, cellCount>;

    StaticRegularSudoku() = default;

    template<std::convertible_to<Integer>... Ints>
        requires (sizeof...(Ints) == cellCount)
    explicit StaticRegularSudoku(Ints&&... ints)
        : m_array{ static_cast<Integer>(ints)... }
    {}

    auto begin() const noexcept { return m_array.begin(); }
    auto end() const noexcept { return m_array.end(); }
    auto begin() noexcept { return m_array.begin(); }
    auto end() noexcept { return m_array.end(); }

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
