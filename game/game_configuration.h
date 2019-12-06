#ifndef ALGORITHMIC_LANGUAGES_PROJECT_2_GAME_CONFIGURATION_H
#define ALGORITHMIC_LANGUAGES_PROJECT_2_GAME_CONFIGURATION_H

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


#endif //ALGORITHMIC_LANGUAGES_PROJECT_2_GAME_CONFIGURATION_H
