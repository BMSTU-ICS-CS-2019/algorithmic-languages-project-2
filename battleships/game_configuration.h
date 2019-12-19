#pragma once

#include <cstddef>

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

        GameConfiguration() : field_width(0), field_height(0) {}

        GameConfiguration(const size_t field_width, const size_t field_height) :
                field_width(field_width), field_height(field_height) {}
    };
}