#include "game/simple_game_field.h"

#include "util/cli_util.h"

using std::cin;
using std::cout;
using std::endl;

struct Coordinate {
    const size_t x, y;
};

Coordinate read_coordinate_safely(const size_t &width, const size_t &height) {
    size_t x, y;
    do {
        cout << "Enter X & Y" << endl;
        char x_char;
        cin >> x_char;
        x = x_char - 'A';
        cin >> y;
    } while ((x < 0 || x >= width) || (y < 0 || y >= height));

    return Coordinate{(size_t) x, y};
}

int main() {
    cli::print_logo();

    battleships::SimpleGameField field = battleships::SimpleGameField(
            battleships::GameConfiguration{10, 10}
    );
    field.emplace_ship(1, 1);
    field.emplace_ship(1, 2);
    field.emplace_ship(1, 3);
    field.emplace_ship(5, 6);
    field.emplace_ship(5, 5);
    field.emplace_ship(9, 9);
    field.print_to_console();
    while (true) {
        const auto coordinate = read_coordinate_safely(10, 10);
        field.attack(coordinate.x, coordinate.y);
        field.print_to_console();
    }
    return 0;
}
