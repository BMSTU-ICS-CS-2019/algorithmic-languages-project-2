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
            ALREADY_ATTACKED, MISS, DAMAGE_SHIP, DESTROY_SHIP, WIN
        };

        virtual ~GameField() = default;

        [[nodiscard]] virtual GameConfiguration get_configuration() const noexcept = 0;

        [[nodiscard]] virtual bool is_discovered(const Coordinate &coordinate) const = 0;

        [[nodiscard]] virtual bool can_be_attacked(const Coordinate &coordinate) const = 0;

        [[nodiscard]] virtual bool can_place_near(const Coordinate &coordinate) const = 0;

        virtual bool try_emplace_ship(const Coordinate &coordinate, const Direction &direction, const size_t &size) = 0;

        virtual AttackStatus attack(const Coordinate &coordinate) noexcept(false) = 0;

        [[nodiscard]] virtual char get_public_icon_at(const Coordinate &coordinate) const = 0;

        [[nodiscard]] virtual bool is_in_bounds(const Coordinate &coordinate) const noexcept = 0;

        [[nodiscard]] virtual bool is_out_of_bounds(const Coordinate &coordinate) const noexcept = 0;

        virtual void reset() noexcept = 0;

        [[nodiscard]] virtual bool can_place_at(const Coordinate &coordinate) const = 0;

        virtual void locate_not_visited_spot(Coordinate &start, Direction direction, const bool &clockwise) const = 0;
    };
}
