#pragma once

#include <stdexcept>

using std::invalid_argument;

namespace battleships {
    enum Direction {
        UP, LEFT, DOWN, RIGHT
    };

    inline static Direction invert(const Direction &direction) {
        switch (direction) {
            case UP: return DOWN;
            case LEFT: return RIGHT;
            case DOWN: return UP;
            case RIGHT: return LEFT;
            default: throw invalid_argument(&"Unknown direction " [direction])
        }
    }
}


