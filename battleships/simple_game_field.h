#pragma once

#include <string>

#include "game_field.h"
#include "game_configuration.h"
#include "coordinate.h"
#include "game_field_cell.h"

using std::string;
using std::to_string;

namespace battleships {

    class SimpleGameField : public GameField {

    protected:

        const GameConfiguration configuration_;

        GameFieldCell ***cells_;

        size_t ship_cells_alive_ = 0;

        inline void check_bounds(const Coordinate &coordinate) const noexcept(false) {
            if (is_out_of_bounds(coordinate))
                throw out_of_range(
                        "Coordinate (" + to_string(coordinate.x)
                        + ":" + to_string(coordinate.y) + ") is out of its range"
                );
        }

        [[nodiscard]] inline GameFieldCell *get_cell_at(const Coordinate &coordinate) const {
            return cells_[coordinate.x][coordinate.y];
        }

        inline void set_cell_at(const Coordinate &coordinate, GameFieldCell *const value) {
            auto &cell = cells_[coordinate.x][coordinate.y];
            delete cell;
            cell = value;
        }

        inline void try_make_discovered(const Coordinate &coordinate);

        inline void surround_ship_cell(const Coordinate &coordinate);

        /**
         * @brief Attempts to destroy the ship by attacking the given point.
         * @param x X-coordinate of the point attacked
         * @param y Y-coordinate of the point attacked
         * @return {@code true} if the ship was fully destroyed by the attack and {@code false} otherwise
         */
        inline bool attempt_destroy_ship(const Coordinate &coordinate);

    public:

        /*
         * Construction and deconstruction
         */

        explicit SimpleGameField(const GameConfiguration &configuration);

        ~SimpleGameField() override;

        /*
         * Data access
         */

        [[nodiscard]] GameConfiguration get_configuration() const noexcept override;

        /*
         * Game logic
         */

        [[nodiscard]] bool is_discovered(const Coordinate &coordinate) const override;

        [[nodiscard]] bool can_place_near(const Coordinate &coordinate) const override;

        bool try_emplace_ship(const Coordinate &base_coordinate,
                              const Direction &direction, const size_t &size) override;

        AttackStatus attack(const Coordinate &coordinate) override;

        /*
         * Misc
         */

        void print_to_console() const noexcept override;

        [[nodiscard]] char get_public_icon_at(const Coordinate &coordinate) const override;

        [[nodiscard]] inline bool is_in_bounds(const Coordinate &coordinate) const noexcept override {
            return (0 <= coordinate.x && coordinate.x < configuration_.field_width)
                   && (0 <= coordinate.y && coordinate.y < configuration_.field_height);
        }

        [[nodiscard]] inline bool is_out_of_bounds(const Coordinate &coordinate) const noexcept override {
            return (coordinate.x < 0 || configuration_.field_width <= coordinate.x)
                   || (coordinate.y < 0 || configuration_.field_height <= coordinate.y);
        }

        void reset() noexcept override;

        [[nodiscard]] bool can_place_at(const Coordinate &coordinate) const override;

        void locate_not_visited_spot(Coordinate &coordinate, Direction direction, const bool &clockwise) const override;
    };
}
