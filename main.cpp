#include "battleships/simple_game_field.h"

#include <string>

#include "util/cli_util.h"
#include "battleships/coordinate.h"
#include "battleships/game_configuration.h"
#include "battleships/simple_game.h"
#include "battleships/simple_rival_bot.h"

using std::cin;
using std::cout;
using std::endl;
using std::pair;
using std::string;

using battleships::Coordinate;
using battleships::GameConfiguration;
using battleships::GameField;
using battleships::SimpleGame;
using battleships::SimpleRivalBot;

Coordinate read_coordinate_safely(const size_t &width, const size_t &height) {
    size_t x, y;
    do {
        cout << "> Enter valid {X,Y} coordinate" << endl;
        char x_char;
        cin >> x_char;
        x = x_char - 'A';
        cin >> y;
    } while ((x < 0 || x >= width) || (y < 0 || y >= height));

    return Coordinate(x, y);
}

GameConfiguration default_game_configuration() {
    GameConfiguration configuration(10, 10, 4);

    for (size_t i = 1; i <= 4; i++) configuration.ship_counts.insert(pair(i, 4 - i + 1));

    return configuration;
}

GameConfiguration read_game_configuration() {
    GameConfiguration configuration;

    cout << "Enter field width" << endl;
    cin >> configuration.field_width;
    cout << "Enter field height" << endl;
    cin >> configuration.field_height;
    // todo read other properties

    return configuration;
}

void read_player_field(GameField * const game_field) {
    while (true) {
        cout << "> Enter valid field configurations" << endl;
        const auto width = game_field->get_configuration().field_width,
                height = game_field->get_configuration().field_height;
        for (const auto &ship_count : game_field->get_configuration().ship_counts) {
            for (size_t ship_id = 1; ship_id <= ship_count.second; ++ship_id) {
                cout << "==================================="
                        << ">> Enter the points of " << ship_count.first << "-celled ship #" << ship_id << endl;

                for (size_t ship_cell_id = 1; ship_cell_id <= ship_count.first; ship_cell_id++) {
                    cout << ">>> Enter ship cell #" << ship_cell_id << endl;
                    game_field->emplace_ship(read_coordinate_safely(width, height));
                    game_field->print_to_console();
                }
            }
        }

        if (game_field->validate()) break;
        else cout << "Something's wrong with your field, try fixing it..." << endl;
    }
}

bool play_against_real_rival() {
    SimpleGame game(default_game_configuration());
    const auto configuration = game.configuration();

    cout << "<< Player 1 >>" << endl;
    read_player_field(game.field_1());
    cout << "<< Player 2 >>" << endl;
    read_player_field(game.field_2());

    bool first_player_turn = true;
    while (true) {
        game.print_to_console();

        cout << (first_player_turn ? "<< Player 1 >>" : "<< Player 2 >>") << endl;

        while (true) {
            const auto coordinate = read_coordinate_safely(configuration.field_width, configuration.field_height);
            const auto attack_status = (first_player_turn ? game.field_2() : game.field_1())->attack(coordinate);
            switch (attack_status) {
                case GameField::ALREADY_ATTACKED: {
                    cout << "> This point has already been attacked" << endl;
                    continue;
                }
                case GameField::MISSED: {
                    cout << "> Player " << (first_player_turn ? "1" : "2") << " missed" << endl;
                    break;
                }
                case GameField::HIT: {
                    cout << "> Player " << (first_player_turn ? "1" : "2") << " has hit a ship!" << endl;
                    continue;
                }
                case GameField::DESTROY: {
                    cout << "> Player " << (first_player_turn ? "1" : "2") << " has destroyed a ship!" << endl;
                    continue;
                }
                case GameField::WIN: {
                    cout << "> Player " << (first_player_turn ? "1" : "2") << " has won this game!" << endl;
                    return true;
                }
                default: throw invalid_argument("Unknown player-attack status");
            }
            break;
        }

        first_player_turn = !first_player_turn;
    }
}

bool play_against_bot_rival() {
    SimpleGame game(default_game_configuration());
    // get the configuration object after the game is created in case it gets modified
    const auto configuration = game.configuration();

    SimpleRivalBot rival_bot(&game);

    while (true) {
        read_player_field(game.field_1());
        if (game.field_1()->validate()) break;
        else game.field_1()->reset();
    }

    bool player_turn = true;
    while (true) {
        game.print_to_console();

        if (player_turn)
            while (true) {
                const auto coordinate = read_coordinate_safely(configuration.field_width, configuration.field_height);
                const auto attack_status = game.field_2()->attack(coordinate);
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
                        cout << "> You've destroyed a ship!" << endl;
                        continue;
                    }
                    case GameField::WIN: {
                        cout << "> You have won this game!" << endl;
                        return true;
                    }
                    default:
                        throw invalid_argument("Unknown player-attack status");
                }
                break;
            }
        else
            switch (rival_bot.act()) {
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
                default:
                    throw invalid_argument("Unknown bot-attack status");
            }

        player_turn = !player_turn;
    }
}

int main() {
    cli::print_logo();

    while (true) {
        cout << "Wanna play?" << endl << endl
                << "Enter `player` to play against the player, `bot` to play against the bot or `exit` to exit" << endl;
        string input;
        cin >> input;
        if (input == "player") play_against_real_rival();
        else if (input == "bot") play_against_bot_rival();
        else if (input == "exit" || input == "quit") return 0;
    }
}
