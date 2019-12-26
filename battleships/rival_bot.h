#pragma once

#include "game_field.h"

namespace battleships  {

    /**
     * @brief Bot responsible for playing against the player
     */
    class RivalBot {

    public:
        virtual void place_ships() = 0;

        virtual bool act() = 0;
    };
}


