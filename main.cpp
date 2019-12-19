#include "battleships/simple_game_field.h"

#include <string>

#include "util/cli_util.h"
#include "battleships/coordinate.h"
#include "battleships/direction.h"
#include "battleships/game_configuration.h"
#include "battleships/simple_game.h"
#include "battleships/simple_rival_bot.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

using battleships::Coordinate;
using battleships::GameConfiguration;
using battleships::GameField;
using battleships::SimpleGame;
using battleships::SimpleRivalBot;

Coordinate read_coordinate_safely(const size_t &width, const size_t &height) {
    size_t x, y;
    do {
        cout << "> Enter X & Y" << endl;
        char x_char;
        cin >> x_char;
        x = x_char - 'A';
        cin >> y;
    } while ((x < 0 || x >= width) || (y < 0 || y >= height));

    return Coordinate(x, y);
}

GameConfiguration read_game_configuration() {
    GameConfiguration configuration;

    cout << "Enter field width" << endl;
    cin >> configuration.field_width;
    cout << "Enter field height" << endl;
    cin >> configuration.field_height;

    return configuration;
}

bool play_round() {
    SimpleGame game(read_game_configuration());
    const auto configuration = game.configuration();

    SimpleRivalBot rival_bot(&game);

    bool player_turn = true;
    while (true) {
        game.print_to_console();

        if (player_turn) while (true) {
            const auto coordinate = read_coordinate_safely(configuration.field_width, configuration.field_height);
            const auto attack_status = game.bot_field()->attack(coordinate);
            switch (attack_status) {
                case GameField::ALREADY_ATTACKED: {
                    cout << "> This point has already been attacked" << endl;
                    continue;
                }
                case GameField::MISSED: {
                    cout << "> You've missed" << endl;
                    break;
                }
                case GameField::HIT: {
                    cout << "> You've hit a ship!" << endl;
                    continue;
                }
                case GameField::DESTROY: {
                    cout << "> You've destroyed the ship!" << endl;
                    continue;
                }
                case GameField::WIN: {
                    cout << "> You have won this game!" << endl;
                    return true;
                }
                default: throw invalid_argument("Unknown player-attack status");
            }
            break;
        } else switch (rival_bot.act()) {
            case GameField::MISSED: {
                cout << "> Bot has missed" << endl;
                break;
            }
            case GameField::HIT: {
                cout << "> Bot has hit your ship!" << endl;
                continue;
            }
            case GameField::DESTROY: {
                cout << "> Bot has destroyed your ship!" << endl;
                continue;
            }
            case GameField::WIN: {
                cout << "> Bot has won this game!" << endl;
                return false;
            }
            default: throw invalid_argument("Unknown bot-attack status");
        }

        player_turn = !player_turn;
    }

}

int main() {
    size_t wins = 0, looses = 0;
    while (true) {
        cout << "Wanna play?" << endl;
        string input;
        cin >> input;
        if (input == "yes" || input == "yep" || input == "1" || input == "+") if (play_round()) ++wins;
        else ++looses; // NOLINT(readability-misleading-indentation)
        else return 0;
    }
    random_device r;
    cout << battleships::random_direction(r);
    cli::print_logo();

    battleships::SimpleGameField field = battleships::SimpleGameField(
            battleships::GameConfiguration{10, 10}
    );
    field.emplace_ship(Coordinate(1, 1));
    field.emplace_ship(Coordinate(1, 2));
    field.emplace_ship(Coordinate(1, 3));
    field.emplace_ship(Coordinate(5, 6));
    field.emplace_ship(Coordinate(5, 5));
    field.emplace_ship(Coordinate(9, 9));
    field.print_to_console();
    while (true) {
        const auto coordinate = read_coordinate_safely(10, 10);
        field.attack(coordinate);
        field.print_to_console();
    }
    return 0;
}
