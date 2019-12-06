#include "game/simple_game_field.h"

#include "util/cli_util.h"

int main() {
    cli::print_logo();

    battleships::SimpleGameField field = battleships::SimpleGameField(
            battleships::GameConfiguration{10, 10}
    );
    field.print_to_console();
    field.emplace_ship(1, 1);
    field.emplace_ship(1, 2);
    field.emplace_ship(1, 3);
    field.emplace_ship(5,6);
    field.emplace_ship(5,5);
    field.emplace_ship(9,9);
    while (true) {
        size_t x, y;
        std::cin >> x >> y;
        field.attack(x, y);
        field.print_to_console();
    }
    return 0;
}
