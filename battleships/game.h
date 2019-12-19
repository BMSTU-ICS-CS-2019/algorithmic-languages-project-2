#pragma once

#include "game_field.h"

namespace battleships  {
    class Game : public ConsolePrintable {

    public:

        [[nodiscard]] virtual GameConfiguration configuration() = 0;

        [[nodiscard]] virtual GameField *player_field() = 0;

        [[nodiscard]] virtual GameField *bot_field() = 0;
    };
}
