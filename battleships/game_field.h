#pragma once

#include "console_printable.h"
#include "coordinate.h"
#include "game_configuration.h"
#include <stdexcept>

using std::out_of_range;
using std::runtime_error;

namespace battleships {
    class GameField : public ConsolePrintable {

    public:
        enum AttackStatus {
            ALREADY_ATTACKED, MISSED, HIT, DESTROY, WIN
        };

        virtual ~GameField() = default;

        [[nodiscard]] virtual GameConfiguration get_configuration() const noexcept = 0;

        virtual bool is_visited(const Coordinate &coordinate) = 0;

        virtual void emplace_ship(const Coordinate &coordinate) noexcept(false) = 0;

        virtual AttackStatus attack(const Coordinate &coordinate) noexcept(false) = 0;

        virtual char get_icon_at(const Coordinate &coordinate) const = 0;
    };
}
