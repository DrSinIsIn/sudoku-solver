// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "Solvers/Utility/SetBitIterator.h"

#include <algorithm>
#include <bitset>

namespace
{
    inline constexpr std::bitset<10> smallEmptyBitset{};
    inline constexpr std::bitset<160> largeEmptyBitset{};
    inline constexpr std::bitset<10> smallBitset{ 0b0010001101 };
    inline const std::bitset<160> largeBitset{ "1001010110111010111011010011010101100011"
                                               "0110100110110010101100011011101011101000"
                                               "1101010110001011101011101101010110010001"
                                               "0000101111001010101111101101101010101010" };
}

TEST(SetBitIteratorTest, smallEmptyBitsetIteration)
{
    SetBitIterator iterator{ ::smallEmptyBitset };
    decltype(iterator) const sentinel{};

    ASSERT_EQ(iterator, sentinel);
}

TEST(SetBitIteratorTest, largeEmptyBitsetIteration)
{
    SetBitIterator iterator{ ::largeEmptyBitset };
    decltype(iterator) const sentinel{};

    ASSERT_EQ(iterator, sentinel);
}

TEST(SetBitIteratorTest, smallBitsetIteration)
{
    SetBitIterator iterator{ ::smallBitset };
    decltype(iterator) const sentinel{};

    ASSERT_NE(iterator, sentinel);

    std::size_t count{};
    for (; iterator != sentinel; ++iterator, ++count)
    {
        // *iterator is the index of the current set bit
        ASSERT_TRUE(::smallBitset.test(*iterator));
    }

    ASSERT_EQ(count, ::smallBitset.count());
}

TEST(SetBitIteratorTest, largeBitsetIteration)
{
    SetBitIterator iterator{ ::largeBitset };
    decltype(iterator) const sentinel{};

    ASSERT_NE(iterator, sentinel);

    std::size_t count{};
    for (; iterator != sentinel; ++iterator, ++count)
    {
        // *iterator is the index of the current set bit
        ASSERT_TRUE(::largeBitset.test(*iterator));
    }

    ASSERT_EQ(count, ::largeBitset.count());
}
