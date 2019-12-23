#include "simple_game_field.h"

#include "container_util.h"
#include <tuple>
#include <set>

using common_util::contains;
using common_util::not_contains;
using std::tuple;
using std::set;

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

    bool SimpleGameField::attempt_destroy_ship(const Coordinate &coordinate) {
        set<Coordinate> ship_cells;
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
                        ship_cells.emplace(floating_axis, coordinate.y);
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
                        ship_cells.emplace(coordinate.x, floating_axis);
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
                        ship_cells.emplace(floating_axis, coordinate.y);
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
                        ship_cells.emplace(coordinate.x, floating_axis);
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
            for (; coordinate.y < configuration_.field_height; ++coordinate.y) if (not_contains(
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
        for (size_t x = 0; x < configuration_.field_width; x++) for (size_t y = 0;
                y < configuration_.field_width; y++) cells_[x][y] = EMPTY;
    }
}