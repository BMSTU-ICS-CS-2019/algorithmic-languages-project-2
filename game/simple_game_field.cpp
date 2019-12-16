#include "simple_game_field.h"

#include <tuple>
#include <vector>

using std::tuple;
using std::vector;

namespace battleships {

    /**
     * @brief Attempts to destroy the ship by attacking the given point.
     * @param x X-coordinate of the point attacked
     * @param y Y-coordinate of the point attacked
     * @return {@code true} if the ship was fully destroyed by the attack and {@code false} otherwise
     */
    bool SimpleGameField::attempt_destroy_ship(const size_t &x, const size_t &y) {
        // go left

        struct CoordinatePair {
            size_t x, y;
        };

        vector<CoordinatePair> ship_cells;
        {
            size_t coordinate = x;
            // go left

            while (true) {
                if (coordinate == 0) break;
                --coordinate;
                switch (cells_[coordinate][y]) {
                    case EMPTY: case VISITED: continue;
                    case OCCUPIED: {
                        cells_[x][y] = ATTACKED;
                        return false;
                    }
                    case ATTACKED: {
                        ship_cells.emplace_back(CoordinatePair{coordinate, y});
                        continue;
                    }
                    case DESTROYED: throw runtime_error("The ship is touching another one");
                }
            }
            // go up
            coordinate = y;
            while (true) {
                if (coordinate == 0) break;
                --coordinate;
                switch (cells_[x][coordinate]) {
                    case EMPTY: case VISITED: continue;
                    case OCCUPIED: {
                        cells_[x][y] = ATTACKED;
                        return false;
                    }
                    case ATTACKED: {
                        ship_cells.emplace_back(CoordinatePair{x, coordinate});
                        continue;
                    }
                    case DESTROYED: throw runtime_error("The ship is touching another one");
                }
            }
            coordinate = x;
            // go right
            while (++coordinate < configuration_.field_width) {
                switch (cells_[coordinate][y]) {
                    case EMPTY: case VISITED: continue;
                    case OCCUPIED: {
                        cells_[x][y] = ATTACKED;
                        return false;
                    }
                    case ATTACKED: {
                        ship_cells.emplace_back(CoordinatePair{coordinate, y});
                        continue;
                    }
                    case DESTROYED: throw runtime_error("The ship is touching another one");
                }
            }
            // go down
            coordinate = y;
            while (++coordinate < configuration_.field_height) {
                switch (cells_[x][coordinate]) {
                    case EMPTY: case VISITED: continue;
                    case OCCUPIED: {
                        cells_[x][y] = ATTACKED;
                        return false;
                    }
                    case ATTACKED: {
                        ship_cells.emplace_back(CoordinatePair{x, coordinate});
                        continue;
                    }
                    case DESTROYED: throw runtime_error("The ship is touching another one");
                }
            }
        }
        cells_[x][y] = DESTROYED;
        for (const auto &ship_coordinate : ship_cells) cells_[ship_coordinate.x][ship_coordinate.y] = DESTROYED;

        return true;
    }

    Game::AttackStatus SimpleGameField::attack(const size_t &x, const size_t &y) {
        check_bounds(x, y);

        switch (cells_[x][y]) {
            // handle repeated attacks
            case VISITED: case ATTACKED: case DESTROYED: return Game::ALREADY_ATTACKED;
            // handle new attacks
            case EMPTY: {
                cells_[x][y] = VISITED;
                return Game::MISSED;
            }
            case OCCUPIED: {
                --ship_cells_alive_;
                if (attempt_destroy_ship(x, y)) return ship_cells_alive_ == 0 ? Game::DESTROY : Game::WIN;
                else return Game::HIT;
            }
        }
    }

    SimpleGameField::~SimpleGameField() {
        for (size_t x = 0; x < configuration_.field_width; x++) delete[] cells_[x];
        delete[] cells_;
    }

    SimpleGameField::SimpleGameField(const GameConfiguration &configuration)
            : configuration_(configuration), cells_(new CellState *[configuration_.field_width]) {
        for (size_t x = 0; x < configuration_.field_width; x++) {
            const auto column = cells_[x] = new CellState[configuration.field_height];

            for (size_t y = 0; y < configuration.field_height; y++) column[y] = CellState::EMPTY;
        }
    }

    void SimpleGameField::print_to_console() const noexcept {
        const auto width = configuration_.field_width, height = configuration_.field_height;
        // draw upper border
        for (size_t i = 0; i < width * 2 + 1; i++) cout << "-";
        cout << endl;

        {
            for (size_t y = 0; y < height; y++) {
                for (size_t x = 0; x < width; x++) {
                    cout << "|" << as_string(cells_[x][y]);
                }
                cout << "|" << endl;
            }
        }

        // draw lower border
        for (size_t i = 0; i < width * 2 + 1; i++) cout << "-";
        cout << endl;
    }

    void SimpleGameField::emplace_ship(const size_t &x, const size_t &y) {
        check_bounds(x, y);
        cells_[x][y] = OCCUPIED;
        ++ship_cells_alive_;
    }
}