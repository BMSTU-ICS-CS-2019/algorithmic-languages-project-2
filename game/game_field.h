#pragma once

#include "console_printable.h"
#include "game.h"
#include <stdexcept>

using std::out_of_range;
using std::runtime_error;

namespace battleships {
    class GameField : public ConsolePrintable {

    public:

        virtual ~GameField() = default;

        virtual void emplace_ship(const size_t &x, const size_t &y) noexcept(false) = 0;

        virtual Game::AttackStatus attack(const size_t &x, const size_t &y) noexcept(false) = 0;
    };
}
