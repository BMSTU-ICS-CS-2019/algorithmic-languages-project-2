#pragma once

#include "game_field.h"

namespace battleships {
    class Game : public ConsolePrintable {

    public:

        virtual ~Game() = default;

        [[nodiscard]] virtual GameConfiguration configuration() = 0;

        [[nodiscard]] virtual GameField *field_1() = 0;

        [[nodiscard]] virtual GameField *field_2() = 0;
    };
}
