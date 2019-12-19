#pragma once

#include <random>
#include <stdexcept>

using std::random_device;
using std::bernoulli_distribution;
using std::invalid_argument;
using std::runtime_error;

namespace battleships {

    static bernoulli_distribution ship_position_bool_distribution; // NOLINT(cert-err58-cpp)

    enum ShipPosition {
        VERTICAL, HORIZONTAL
    };

    inline static ShipPosition invert_ship_position(const ShipPosition &ship_position) {
        switch (ship_position) {
            case VERTICAL: return HORIZONTAL;
            case HORIZONTAL: return VERTICAL;
            default: throw invalid_argument(&"Unknown ship position " [ship_position]);
        }
    }

    inline static ShipPosition random_direction(random_device &random) {
        return ship_position_bool_distribution(random) ? VERTICAL : HORIZONTAL;
    }
}


