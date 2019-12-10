#pragma once

#include "game_field.h"
#include "game_configuration.h"


namespace battleships {

    class SimpleGameField : public GameField {

    protected:

        enum CellState {
            EMPTY, VISITED, OCCUPIED, ATTACKED, DESTROYED,
        };

        inline static char as_char(const CellState& cell_state) {
            switch (cell_state) {
                case EMPTY: return '~';
                case VISITED: return '-';
                case OCCUPIED: return '#';
                case ATTACKED: return '+';
                case DESTROYED: return 'X';
            }
        }

        inline static std::string as_string(const CellState& cell_state) {
            switch (cell_state) {
                case EMPTY: return "~";
                case VISITED: return "-";
                case OCCUPIED: return "#";
                case ATTACKED: return "+";
                case DESTROYED: return "X";
            }
        }

        /**
         * @brief Attempts to destroy the ship by attacking the given point.
        * @param x X-coordinate of the point attacked
        * @param y Y-coordinate of the point attacked
        * @return {@code true} if the ship was fully destroyed by the attack and {@code false} otherwise
        */
        inline bool attempt_destroy_ship(const size_t &x, const size_t &y);

        const GameConfiguration configuration_;

        CellState **cells_;

        size_t ship_cells_alive_ = 0;

        inline void check_bounds(const size_t &x, const size_t &y) noexcept(false) {
            if (x >= configuration_.field_width || y >= configuration_.field_height) throw out_of_range(
                    "Attacked coordinate is out of its range"
            );
        }

    public:

        ~SimpleGameField() override;

        void emplace_ship(const size_t &x, const size_t &y) override;

        GameField::AttackStatus attack(const size_t &x, const size_t &y) override;

        void print_to_console() const noexcept override;

        explicit SimpleGameField(const battleships::GameConfiguration &configuration);
    };
}
