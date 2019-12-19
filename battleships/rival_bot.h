#pragma once

#include "game_field.h"

namespace battleships  {

    /**
     * @brief Bot responsible for playing against the player
     */
    class RivalBot {

    public:
        virtual GameField::AttackStatus act() = 0;
    };
}


