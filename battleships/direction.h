#pragma once

#include <random>
#include <stdexcept>

using std::random_device;
using std::uniform_int_distribution;
using std::invalid_argument;
using std::runtime_error;

namespace battleships {

    static uniform_int_distribution<int8_t> direction_int_distribution(0, 3); // NOLINT(cert-err58-cpp)
    static uniform_int_distribution<int8_t> horizontal_direction_int_distribution(0, 1); // NOLINT(cert-err58-cpp)
    static uniform_int_distribution<int8_t> vertical_direction_int_distribution(0, 1); // NOLINT(cert-err58-cpp)

    enum Direction {
        RIGHT, DOWN, LEFT, UP
    };

    const static Direction ALL_DIRECTIONS[] = {
            RIGHT, DOWN, LEFT, UP
    };

    inline static Direction invert_direction(const Direction &direction) {
        switch (direction) {
            case RIGHT: return LEFT;
            case DOWN: return UP;
            case LEFT: return RIGHT;
            case UP: return DOWN;
            default: throw invalid_argument(&"Unknown direction " [direction]);
        }
    }

    inline static bool is_horizontal_direction(const Direction &direction) {
        switch (direction) {
            case RIGHT: case LEFT: return true;
            case UP: case DOWN: return false;
            default: throw invalid_argument(&"Unknown direction " [direction]);
        }
    }

    inline static bool is_vertical_direction(const Direction &direction) {
        switch (direction) {
            case RIGHT: case LEFT: return false;
            case UP: case DOWN: return true;
            default: throw invalid_argument(&"Unknown direction " [direction]);
        }
    }

    inline static Direction rotate_direction_clockwise(const Direction &direction) {
        switch (direction) {
            case RIGHT: return DOWN;
            case DOWN: return LEFT;
            case LEFT: return UP;
            case UP: return RIGHT;
            default: throw invalid_argument(&"Unknown direction " [direction]);
        }
    }

    inline static Direction rotate_direction_counter_clockwise(const Direction &direction) {
        switch (direction) {
            case RIGHT: return UP;
            case DOWN: return RIGHT;
            case LEFT: return DOWN;
            case UP: return LEFT;
            default: throw invalid_argument(&"Unknown direction " [direction]);
        }
    }

    inline static Direction random_direction(random_device &random) {
        switch (direction_int_distribution(random)) {
            case 0: return RIGHT;
            case 1: return DOWN;
            case 2: return LEFT;
            case 3: return UP;
            default: throw runtime_error("Generated random value is out of range");
        }
    }

    inline static Direction random_horizontal_direction(random_device &random) {
        switch (horizontal_direction_int_distribution(random)) {
            case 0: return RIGHT;
            case 1: return LEFT;
            default: throw runtime_error("Generated random value is out of range");
        }
    }

    inline static Direction random_vertical_direction(random_device &random) {
        switch (vertical_direction_int_distribution(random)) {
            case 0: return DOWN;
            case 1: return UP;
            default: throw runtime_error("Generated random value is out of range");
        }
    }
}


