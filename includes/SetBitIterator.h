// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bit>
#include <climits>
#include <cstddef>

template<typename Bitset>
class SetBitIterator
{
public:
    explicit SetBitIterator(Bitset const& b = Bitset{})
        : m_remaining{ b }
    {
        goToNext();
    }

    constexpr bool operator==(SetBitIterator const&) const = default;

    auto& operator++()
    {
        m_index += 1;
        m_remaining >>= 1;
        goToNext();
        return *this;
    }

    std::size_t operator*() const
    {
        return m_index;
    }

private:
    Bitset m_remaining;
    std::size_t m_index = {};

    void goToNext()
    {
        if (m_remaining.none())
        {
            m_index = m_remaining.size();
            return;
        }

        constexpr auto ullWidth = sizeof(unsigned long long) * CHAR_BIT;
        constexpr Bitset ullMask = ~0ull;

        Bitset nextULLBitset;
        while ((nextULLBitset = (m_remaining & ullMask)).none())
        {
            m_index += ullWidth;
            m_remaining >>= ullWidth;
        }

        auto const asULL = nextULLBitset.to_ullong();
        auto const countZeros = std::countr_zero(asULL);

        m_index += countZeros;
        m_remaining >>= countZeros;
    }
};
