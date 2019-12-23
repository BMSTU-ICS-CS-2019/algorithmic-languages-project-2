#include "simple_game_field.h"

#include "container_util.h"
#include <tuple>
#include <set>

using common_util::contains;
using common_util::not_contains;
using std::tuple;
using std::pair;
using std::set;

namespace battleships {

    /*
     * Construction and deconstruction
     */

    SimpleGameField::SimpleGameField(const GameConfiguration &configuration)
            : configuration_(configuration), cells_(new GameFieldCell **[configuration_.field_width]) {
        for (size_t x = 0; x < configuration_.field_width; x++) {
            const auto column = cells_[x] = new GameFieldCell *[configuration.field_height];

            for (size_t y = 0; y < configuration.field_height; y++) column[y] = new EmptyGameFieldCell;
        }
    }

    SimpleGameField::~SimpleGameField() {
        for (size_t x = 0; x < configuration_.field_width; x++) {
            const auto line = cells_[x];
            for (size_t y = 0; y < configuration_.field_height; y++) delete line[y];
            delete[] line;
        }
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

    void SimpleGameField::try_make_discovered(const Coordinate &coordinate) {
        if (is_in_bounds(coordinate)) {
            const auto cell = get_cell_at(coordinate);
            if (cell->is_empty()) cell->discover();
        }
    }

    void SimpleGameField::make_destroyed(const Coordinate &coordinate) {
        try_make_discovered(coordinate.move(RIGHT, 1));
        try_make_discovered(coordinate.move(LEFT, 1));

        const auto up = coordinate.move(UP, 1);
        const auto down = coordinate.move(DOWN, 1);
        try_make_discovered(up);
        try_make_discovered(down);

        try_make_discovered(up.move(RIGHT, 1));
        try_make_discovered(up.move(LEFT, 1));
        try_make_discovered(down.move(RIGHT, 1));
        try_make_discovered(down.move(LEFT, 1));
    }

    bool SimpleGameField::attempt_destroy_ship(const Coordinate &coordinate) {
        const auto cell = get_cell_at(coordinate);

        if (!cell->is_empty()) throw runtime_error("Attempt to destroy a cell not being a ship");

        const auto ship_cell = (ShipGameFieldCell*) cell;

        const auto position = ship_cell->get_position();
        if (position == NONE) {
            // simply destroy the current point as it is a small ship
            ship_cell->discover();
            make_destroyed(coordinate);
        } else {
            // map of other cells which correspond to this ship
            map<Coordinate, GameFieldCell*> destroyed_cells;
            {
                size_t ship_cells_left = ship_cell->get_ship_size() - 1;
                auto direction = position == VERTICAL ? UP : RIGHT;
                for (size_t i = 0; i < 2; i++) { // go in both directions
                    if (ship_cells_left == 0) break; // there is no need to test for any other ship cells
                    auto tested_coordinate = coordinate;
                    while (true) {
                        tested_coordinate.move(direction, 1); // mark the current cell as the one destroyed
                        if (!is_in_bounds(tested_coordinate)) break; // border reached

                        const auto tested_cell = get_cell_at(tested_coordinate);
                        if (tested_cell->is_empty()) break; // end of ship reached
                        if (tested_cell->is_discovered()) {
                            destroyed_cells.insert(pair<Coordinate, GameFieldCell*>(tested_coordinate, tested_cell));
                            if (--ship_cells_left == 0) break; // there is no need to test for any other ship cells
                        } else return false; // the ship is not yet fully destroyed
                    }

                    direction = invert_direction(direction);
                }
            }

            // destroy the attacked cell of the ship
            ship_cell->discover();
            try_make_discovered(coordinate);
            // destroy each other cells of this ship
            for (const auto &destroyed_cell : destroyed_cells) {
                destroyed_cell.second->discover();
                try_make_discovered(destroyed_cell.first);
            }

            return true;
        }
    }

    /*
     * Game logic
     */

    GameField::AttackStatus SimpleGameField::attack(const Coordinate &coordinate) {
        check_bounds(coordinate);

        const auto cell = get_cell_at(coordinate);

        if (cell->is_empty()) {
            if (cell->is_discovered()) return ALREADY_ATTACKED;
            else {
                cell->discover();
                return MISSED;
            }
        } else {
            get_cell_at(coordinate);
        }
        switch () {
            // handle repeated attacks
            case VISITED:
            case ATTACKED:
            case DESTROYED:
                return ALREADY_ATTACKED;
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

        return get_cell_at(coordinate)->is_discovered();
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
        cout << "\r\n";

        {
            auto number = 0;
            for (size_t y = 0; y < height; y++) {
                cout << number++ << '|';
                for (size_t x = 0; x < width; x++) {
                    cout << cell_state_as_string(cells_[x][y]) << '|';
                }
                cout << "\r\n";
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

            case EMPTY:
            case OCCUPIED:
                return '~'; // unknown
            case VISITED:
                return '-';
            case ATTACKED:
                return '+';
            case DESTROYED:
                return 'X';
            default:
                throw invalid_argument("Unknown cell state");
        }
    }

    void SimpleGameField::locate_not_visited_spot(Coordinate &coordinate, Direction direction,
                                                  const bool &clockwise) {
        if (is_visited(coordinate)) {
            size_t step_count = 0;

            bool last_was_without_steps = false;
            while (true) { // this will finally fail
                ++step_count;
                bool made_no_steps = true;

                for (size_t attempt = 0; attempt < 2; attempt++) {
                    for (size_t i = 0; i < step_count; i++) {
                        coordinate.move(direction, step_count);
                        if (is_in_bounds(coordinate)) {
                            if (!is_visited(coordinate)) return;
                            made_no_steps = false;
                        } else coordinate.move(direction, -step_count); // undo
                    }

                    // make a rotation
                    direction = clockwise ? next_clockwise(direction) : next_counter_clockwise(direction);
                }

                // check can be done right now
                if (made_no_steps) {
                    if (last_was_without_steps) throw runtime_error("The game has no free spots");
                    else last_was_without_steps = true;
                } else last_was_without_steps = false;
            }
        }
    }

    bool SimpleGameField::validate() const {
        const auto ship_cell_count = configuration_.ship_cell_count();
        if (ship_cells_alive_ != ship_cell_count) return false;

        set<Coordinate> skipped_coordinates;
        map<size_t, size_t> ship_counts;
        for (size_t x = 0; x < configuration_.field_width; ++x) {
            Coordinate coordinate(x, 0);
            for (; coordinate.y < configuration_.field_height; ++coordinate.y)
                if (not_contains(
                        skipped_coordinates, coordinate)) {
                    if (get_cell_at(coordinate) == CellState::OCCUPIED) {
                        // check sizes and other data
                        if (x + 1 < configuration_.field_width) {
                            // try horizontally
                            auto otherX = x;
                            size_t ship_size = 1;
                        } else if (coordinate.y + 1 < configuration_.field_height) {
                            // try vertically
                        }
                    }
                }
        }

        return ship_counts != configuration_.ship_counts;
    }

    void SimpleGameField::reset() noexcept {
        for (size_t x = 0; x < configuration_.field_width; x++)
            for (size_t y = 0;
                 y < configuration_.field_width; y++)
                cells_[x][y] = EMPTY;
    }
}