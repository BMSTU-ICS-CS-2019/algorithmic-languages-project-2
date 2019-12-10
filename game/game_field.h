#pragma once

#include "console_printable.h"
#include <stdexcept>

using std::out_of_range;
using std::runtime_error;

namespace battleships {
    class GameField : public ConsolePrintable {

    public:

        virtual ~GameField() = default;

        enum AttackStatus {
            ALREADY_ATTACKED, MISSED, HIT, DESTROY, WIN
        };

        virtual void emplace_ship(const size_t &x, const size_t &y) noexcept(false) = 0;

        virtual GameField::AttackStatus attack(const size_t &x, const size_t &y) noexcept(false) = 0;
    };
}
