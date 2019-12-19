#pragma once

#include <string>

#include "game_field.h"
#include "game_configuration.h"
#include "coordinate.h"

using std::string;
using std::to_string;

namespace battleships  {

    class SimpleGameField : public GameField {

    protected:

        enum CellState {
            EMPTY, VISITED, OCCUPIED, ATTACKED, DESTROYED,
        };

        inline static char cell_state_as_char(const CellState &cell_state) {
            switch (cell_state) {
                case EMPTY: return '~';
                case VISITED: return '-';
                case OCCUPIED: return '#';
                case ATTACKED: return '+';
                case DESTROYED: return 'X';
            }
        }

        inline static std::string cell_state_as_string(const CellState &cell_state) {
            switch (cell_state) {
                case EMPTY: return "~";
                case VISITED: return "-";
                case OCCUPIED: return "#";
                case ATTACKED: return "+";
                case DESTROYED: return "X";
            }
        }

        const GameConfiguration configuration_;

        CellState **cells_;

        size_t ship_cells_alive_ = 0;

        inline void check_bounds(const Coordinate &coordinate) const noexcept(false)  {
            if (coordinate.x >= configuration_.field_width
                    || coordinate.y >= configuration_.field_height) throw out_of_range(
                            "Attacked coordinate (" + to_string(coordinate.x)
                            + ":" + to_string(coordinate.y) + ") is out of its range"
                );
        }

        [[nodiscard]] inline CellState get_cell_at(const Coordinate &coordinate) const {
            return cells_[coordinate.x][coordinate.y];
        }

        inline void set_cell_at(const Coordinate &coordinate, const CellState &state) {
            cells_[coordinate.x][coordinate.y] = state;
        }

        inline void try_make_visited(const Coordinate &coordinate);

        inline void try_surround_destroyed(const Coordinate &coordinate);

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

        bool is_visited(const Coordinate &coordinate) override;

        void emplace_ship(const Coordinate &coordinate) override;

        AttackStatus attack(const Coordinate &y) override;

        /*
         * Misc
         */

        void print_to_console() const noexcept override;

        char get_icon_at(const Coordinate &coordinate) const override;
    };
}
