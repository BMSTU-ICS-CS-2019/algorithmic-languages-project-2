#pragma once

#include <cstddef>
#include "direction.h"

namespace battleships  {

    struct Coordinate {
        size_t x, y;

        inline Coordinate(const size_t &x, const size_t &y) : x(x), y(y) {}

        void move(const Direction &direction, const size_t &delta) {
            switch (direction) {
                case RIGHT: {
                    y += delta;
                    break;
                }
                case DOWN: {
                    y -= delta;
                    break;
                }
                case LEFT: {
                    x -= delta;
                    break;
                }
                case UP: {
                    y += delta;
                    break;
                }
                default: throw invalid_argument(&"Unknown direction" [direction]);
            }
        }

        [[nodiscard]] Coordinate move(const Direction &direction, const size_t &delta) const {
            switch (direction) {
                case RIGHT: return Coordinate(x + delta, y);
                case DOWN: return Coordinate(x, y - delta);
                case LEFT: return Coordinate(x - delta, y);
                case UP: return Coordinate(x, y + delta);
                default: throw invalid_argument(&"Unknown direction" [direction]);
            }
        }

        bool operator==(const Coordinate &other) const {
            return x == other.x && y == other.y;
        }

        bool operator>(const Coordinate &other) const {
            return x == other.x ? y > other.y : x > other.x;
        }

        bool operator<(const Coordinate &other) const {
            return x == other.x ? y < other.y : x < other.x;
        }
    };
}


