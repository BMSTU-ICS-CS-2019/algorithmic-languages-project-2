#pragma once

#include <cstddef>

namespace battleships {

    struct Coordinate {
        const size_t x, y;

        inline Coordinate(const size_t &x, const size_t &y): x(x), y(y) {}
    };
}


