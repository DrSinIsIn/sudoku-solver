// Copyright 2022 DrSinIsIn (axel.gaillard.dev@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "Utility/SudokuDescriptor.h"

template<typename Grid>
class AbstractSolver
{
public:
    using GridDescriptor = SudokuDescriptor<Grid>;
    using Bitset = typename GridDescriptor::Bitset;
    using Integer = typename Grid::Integer;

    virtual bool solveOnce(GridDescriptor& gridDescriptor) = 0;
};
