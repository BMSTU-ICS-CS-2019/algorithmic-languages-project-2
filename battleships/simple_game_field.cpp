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

    void SimpleGameField::surround_ship_cell(const Coordinate &coordinate) {
        try_make_discovered(coordinate.move(RIGHT, 1));
        try_make_discovered(coordinate.move(LEFT, 1));

        const auto up = coordinate.move(UP, 1), down = coordinate.move(DOWN, 1);
        try_make_discovered(up);
        try_make_discovered(down);

        try_make_discovered(up.move(RIGHT, 1));
        try_make_discovered(up.move(LEFT, 1));
        try_make_discovered(down.move(RIGHT, 1));
        try_make_discovered(down.move(LEFT, 1));
    }

    bool SimpleGameField::attempt_destroy_ship(const Coordinate &coordinate) {
        const auto cell = get_cell_at(coordinate);

        if (cell->is_empty()) throw runtime_error("Attempt to destroy a cell not being a ship");

        const auto ship_cell = (ShipGameFieldCell*) cell;

        const auto position = ship_cell->get_position();
        if (position == NONE) {
            // simply destroy the current point as it is a small ship
            ship_cell->discover();
            surround_ship_cell(coordinate);
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
                surround_ship_cell(destroyed_cell.first);
            }

            return true;
        }

        return false;
    }

    /*
     * Game logic
     */


    GameField::AttackStatus SimpleGameField::attack(const Coordinate &coordinate) {
        check_bounds(coordinate);

        const auto cell = get_cell_at(coordinate);

        if (cell->is_discovered()) return ALREADY_ATTACKED;
        cell->discover();

        if (cell->is_empty()) return MISS;

        attempt_destroy_ship(coordinate);
        --ship_cells_alive_;

        return ship_cells_alive_ == 0 ? WIN : DESTROY_SHIP;
    }

    bool SimpleGameField::is_discovered(const Coordinate &coordinate) const {
        check_bounds(coordinate);

        return get_cell_at(coordinate)->is_discovered();
    }


    bool SimpleGameField::try_emplace_ship(const Coordinate &base_coordinate,
                                           const Direction &direction, const size_t &size) {

        check_bounds(base_coordinate);
        // check if the ship firs according to the borders

        if (!can_place_at(base_coordinate)) return false;

        if (size == 1) {
            set_cell_at(base_coordinate, new ShipGameFieldCell(size, NONE));
            ++ship_cells_alive_;

            return true;
        }

        {
            auto checked_coordinate = base_coordinate.move(direction, size - 1);
            if (is_out_of_bounds(checked_coordinate)) return false;

            do {
                if (!can_place_at(checked_coordinate)) return false;
                checked_coordinate.move(direction, -1);
            } while (checked_coordinate != base_coordinate);
        }

        // to start from
        for (size_t i = 0; i < size; ++i) set_cell_at(base_coordinate.move(direction, i), new ShipGameFieldCell(
                size, is_vertical_direction(direction) ? VERTICAL : HORIZONTAL
        ));
        ship_cells_alive_ += size;

        return true;
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
        cout << "\n";

        {
            auto number = 0;
            for (size_t y = 0; y < height; y++) {
                cout << number++ << '|';
                for (size_t x = 0; x < width; x++) {
                    cout << cells_[x][y]->private_icon() << '|';
                }
                cout << "\n";
            }
        }

        // draw lower border
        cout << ' ';
        for (size_t i = 0; i < width * 2 + 1; i++) cout << "¯";
        cout << endl;
    }

    char SimpleGameField::get_public_icon_at(const Coordinate &coordinate) const {
        check_bounds(coordinate);

        return get_cell_at(coordinate)->public_icon();
    }

    void SimpleGameField::locate_not_visited_spot(Coordinate &coordinate, Direction direction,
                                                  const bool &clockwise) const {
        if (is_discovered(coordinate)) {
            size_t step_count = 0;

            bool last_was_without_steps = false;
            while (true) { // this will finally fail
                ++step_count;
                bool made_no_steps = true;

                for (size_t attempt = 0; attempt < 2; attempt++) {
                    for (size_t i = 0; i < step_count; i++) {
                        coordinate.move(direction, step_count);
                        if (is_in_bounds(coordinate)) {
                            if (!is_discovered(coordinate)) return;
                            made_no_steps = false;
                        } else coordinate.move(direction, -step_count); // undo
                    }

                    // make a rotation
                    direction = clockwise ? rotate_direction_clockwise(direction) : rotate_direction_counter_clockwise(
                            direction);
                }

                // check can be done right now
                if (made_no_steps) {
                    if (last_was_without_steps) throw runtime_error("The game has no free spots");
                    else last_was_without_steps = true;
                } else last_was_without_steps = false;
            }
        }
    }

    void SimpleGameField::reset() noexcept {
        for (size_t x = 0; x < configuration_.field_width; x++) for (size_t y = 0;
                y < configuration_.field_width; y++) set_cell_at(
                        Coordinate(x, y), new EmptyGameFieldCell
        );
    }

    bool SimpleGameField::can_place_near(const Coordinate &coordinate) const {
        return is_out_of_bounds(coordinate) || get_cell_at(coordinate)->is_empty();
    }

    bool SimpleGameField::can_place_at(const Coordinate &coordinate) const {
        check_bounds(coordinate);

        if (!get_cell_at(coordinate)->is_empty()) return false;

        // check each side
        for (const auto &tested_direction : ALL_DIRECTIONS) {
            auto tested_coordinate = coordinate;

            tested_coordinate.move(tested_direction, 1);
            if (!can_place_near(tested_coordinate)) return false;

            tested_coordinate.move(rotate_direction_clockwise(tested_direction), 1);
            if (!can_place_near(tested_coordinate)) return false;
        }

        return true;
    }
}