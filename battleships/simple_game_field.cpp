#include "simple_game_field.h"

#include <tuple>
#include <vector>

using std::tuple;
using std::vector;

namespace battleships  {

    /*
     * Construction and deconstruction
     */

    SimpleGameField::SimpleGameField(const GameConfiguration &configuration)
            : configuration_(configuration), cells_(new CellState *[configuration_.field_width]) {
        for (size_t x = 0; x < configuration_.field_width; x++) {
            const auto column = cells_[x] = new CellState[configuration.field_height];

            for (size_t y = 0; y < configuration.field_height; y++) column[y] = CellState::EMPTY;
        }
    }

    SimpleGameField::~SimpleGameField() {
        for (size_t x = 0; x < configuration_.field_width; x++) delete[] cells_[x];
        delete[] cells_;
    }

    /*
     * Data access
     */

    GameConfiguration SimpleGameField::get_configuration() const noexcept {
        return configuration_;
    }

    /*
     * Internal methods
     */

    void SimpleGameField::try_make_visited(const Coordinate &coordinate) {
        const auto x = coordinate.x, y = coordinate.y;
        if ((0 <= x && x < configuration_.field_width) && (0 <= y && y < configuration_.field_height)
            && get_cell_at(coordinate) == EMPTY) cells_[x][y] = VISITED;
    }

    void SimpleGameField::try_surround_destroyed(const Coordinate &coordinate) {
        try_make_visited(coordinate.move(RIGHT, 1));
        try_make_visited(coordinate.move(LEFT, 1));

        const auto up = coordinate.move(UP, 1);
        const auto down = coordinate.move(DOWN, 1);
        try_make_visited(up);
        try_make_visited(down);

        try_make_visited(up.move(RIGHT, 1));
        try_make_visited(up.move(LEFT, 1));
        try_make_visited(down.move(RIGHT, 1));
        try_make_visited(down.move(LEFT, 1));
    }

    /**
     * @brief Attempts to destroy the ship by attacking the given point.
     * @param x X-coordinate of the point attacked
     * @param y Y-coordinate of the point attacked
     * @return {@code true} if the ship was fully destroyed by the attack and {@code false} otherwise
     */
    bool SimpleGameField::attempt_destroy_ship(const Coordinate &coordinate) {

        vector<Coordinate> ship_cells;
        {
            size_t floating_axis = coordinate.x;
            // go left
            while (true) {
                if (floating_axis == 0) break;
                --floating_axis;
                switch (cells_[floating_axis][coordinate.x]) {
                    case EMPTY: case VISITED: continue;
                    case OCCUPIED: {
                        set_cell_at(coordinate, ATTACKED);;
                        return false;
                    }
                    case ATTACKED: {
                        ship_cells.emplace_back(floating_axis, coordinate.y);
                        continue;
                    }
                    case DESTROYED: throw runtime_error("The ship is touching another one");
                }
            }
            // go up
            floating_axis = coordinate.y;
            while (true) {
                if (floating_axis == 0) break;
                --floating_axis;
                switch (cells_[coordinate.x][floating_axis]) {
                    case EMPTY: case VISITED: continue;
                    case OCCUPIED: {
                        set_cell_at(coordinate, ATTACKED);;
                        return false;
                    }
                    case ATTACKED: {
                        ship_cells.emplace_back(coordinate.x, floating_axis);
                        continue;
                    }
                    case DESTROYED: throw runtime_error("The ship is touching another one");
                }
            }
            floating_axis = coordinate.x;
            // go right
            while (++floating_axis < configuration_.field_width) {
                switch (cells_[floating_axis][coordinate.y]) {
                    case EMPTY: case VISITED: continue;
                    case OCCUPIED: {
                        set_cell_at(coordinate, ATTACKED);;
                        return false;
                    }
                    case ATTACKED: {
                        ship_cells.emplace_back(floating_axis, coordinate.y);
                        continue;
                    }
                    case DESTROYED: throw runtime_error("The ship is touching another one");
                }
            }
            // go down
            floating_axis = coordinate.y;
            while (++floating_axis < configuration_.field_height) {
                switch (cells_[coordinate.x][floating_axis]) {
                    case EMPTY: case VISITED: continue;
                    case OCCUPIED: {
                        set_cell_at(coordinate, ATTACKED);;
                        return false;
                    }
                    case ATTACKED: {
                        ship_cells.emplace_back(coordinate.x, floating_axis);
                        continue;
                    }
                    case DESTROYED: throw runtime_error("The ship is touching another one");
                }
            }
        }
        {
            set_cell_at(coordinate, DESTROYED);;
            try_surround_destroyed(coordinate);

            for (const auto &ship_coordinate : ship_cells) {
                set_cell_at(ship_coordinate, DESTROYED);;
                try_surround_destroyed(ship_coordinate);
            }
        }

        return true;
    }

    /*
     * Game logic
     */

    GameField::AttackStatus SimpleGameField::attack(const Coordinate &coordinate) {
        check_bounds(coordinate);

        switch (get_cell_at(coordinate)) {
            // handle repeated attacks
            case VISITED: case ATTACKED: case DESTROYED: return ALREADY_ATTACKED;
            // handle new attacks
            case EMPTY: {
                set_cell_at(coordinate, VISITED);;
                return MISSED;
            }
            case OCCUPIED: {
                --ship_cells_alive_;
                if (attempt_destroy_ship(coordinate)) return ship_cells_alive_ == 0 ? DESTROY : WIN;
                else return HIT;
            }
        }
    }

    bool SimpleGameField::is_visited(const Coordinate &coordinate) {
        check_bounds(coordinate);

        const auto cell = get_cell_at(coordinate);

        return cell == VISITED || cell == ATTACKED || cell == DESTROYED;
    }

    void SimpleGameField::emplace_ship(const Coordinate &coordinate) {
        check_bounds(coordinate);

        set_cell_at(coordinate, OCCUPIED);;
        ++ship_cells_alive_;
    }

    /*
     * Misc
     */

    void SimpleGameField::print_to_console() const noexcept {
        const auto width = configuration_.field_width, height = configuration_.field_height;
        // draw upper border
        {
            cout << ' ';
            auto letter = 'A';
            for (size_t i = 0; i < width * 2 + 1; i++) cout << (i % 2 == 0 ? '|' : letter++);
        }
        cout << endl;

        {
            auto number = 0;
            for (size_t y = 0; y < height; y++) {
                cout << number++ << '|';
                for (size_t x = 0; x < width; x++) {
                    cout << cell_state_as_string(cells_[x][y]) << '|';
                }
                cout << endl;
            }
        }

        // draw lower border
        cout << ' ';
        for (size_t i = 0; i < width * 2 + 1; i++) cout << "¯";
        cout << endl;
    }

    char SimpleGameField::get_icon_at(const Coordinate &coordinate) const {
        check_bounds(coordinate);

        switch (get_cell_at(coordinate)) {

            case EMPTY: case OCCUPIED: return '~'; // unknown
            case VISITED: return '-';
            case ATTACKED: return '+';
            case DESTROYED: return 'X';
            default: throw invalid_argument("Unknown cell state");
        }
    }
}