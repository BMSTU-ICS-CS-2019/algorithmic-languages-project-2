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
        const size_t field_width;

        /**
         * @brief Height of the field
         */
        const size_t field_height;

        GameConfiguration(const size_t field_width, const size_t field_height) :
                field_width(field_width), field_height(field_height) {}
    };
}
