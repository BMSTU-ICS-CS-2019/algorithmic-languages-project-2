#pragma once

#include <cstddef>
#include <map>

using std::map;

namespace battleships {

    /**
    * @brief Configuration of a game
    */
    struct GameConfiguration {

        /**
         * @brief Width of the field
         */
        size_t field_width;

        /**
         * @brief Height of the field
         */
        size_t field_height;

        /**
         * @brief Maximal length of the ship
         */
        size_t max_ship_length;

        /**
         * @brief Counts of ships by their length
         */
        map<size_t, size_t> ships;

        GameConfiguration() : field_width(0), field_height(0), max_ship_length(0) {}

        GameConfiguration(const size_t &field_width, const size_t &field_height, const size_t &max_ship_length) :
                field_width(field_width), field_height(field_height), max_ship_length(max_ship_length) {}

        [[nodiscard]] size_t ship_cell_count() const {
            size_t ship_cell_count = 0;

            for (const auto &ship_count : ships) ship_cell_count += ship_count.first * ship_count.second;

            return ship_cell_count;
        }

        bool are_ships_valid() {
            size_t max_ship_coverage = 0;
            for (const auto entry : ships) max_ship_coverage += entry.second * (6 + entry.first * 2);

            return max_ship_coverage <= field_width * field_height;
        }
    };
}
