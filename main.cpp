#include "battleships/simple_game_field.h"

#include <string>
#include <termcap.h>

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
using battleships::Direction;
using battleships::GameConfiguration;
using battleships::GameField;
using battleships::SimpleGame;
using battleships::SimpleRivalBot;

Coordinate read_coordinate_safely(const size_t &width, const size_t &height) {
    size_t x, y;
    do {
        cout << ">>> Enter valid {X,Y} coordinate" << endl;
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

void read_player_field(GameField * const game_field) {
    while (true) {
        cout << "> Enter valid field configurations" << endl;
        const auto width = game_field->get_configuration().field_width,
                height = game_field->get_configuration().field_height;
        for (const auto &ship_count_entry : game_field->get_configuration().ship_counts) {
            for (size_t ship_id = 1; ship_id <= ship_count_entry.second; /* count of such ships */ ++ship_id) {
                cout << ">> Place " << ship_count_entry.first << "-celled ship ["
                        << ship_id << '/' << ship_count_entry.second << ']'<< endl;

                bool placed_successfully;
                do {
                    cout << ">>> Enter the location of your ship's head" << endl;
                    const auto ship_head = read_coordinate_safely(width, height);

                    Direction ship_direction;
                    if (ship_count_entry.first == 1) ship_direction = battleships::UP; // doesn't matter
                    else {
                        cout << ">>> Enter direction of your ship (`(u)p`, `(r)ight`, `(d)own` or `(l)eft`)" << endl;
                        string direction_string;
                        while (true) {
                            cin >> direction_string;
                            if (direction_string == "u" || direction_string == "up") {
                                ship_direction = battleships::DOWN; // due to visual inversion
                                break;
                            }
                            else if (direction_string == "r" || direction_string == "right") {
                                ship_direction = battleships::RIGHT;
                                break;
                            }
                            else if (direction_string == "d" || direction_string == "down") {
                                ship_direction = battleships::UP; // due to visual inversion
                                break;
                            }
                            else if (direction_string == "l" || direction_string == "left") {
                                ship_direction = battleships::LEFT;
                                break;
                            }
                        }
                    }
                    placed_successfully
                            = game_field->try_emplace_ship(ship_head, ship_direction, ship_count_entry.first);
                } while (!placed_successfully);
                game_field->print_to_console();
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
                case GameField::MISS: {
                    cout << "> Player " << (first_player_turn ? "1" : "2") << " missed" << endl;
                    break;
                }
                case GameField::DAMAGE_SHIP: {
                    cout << "> Player " << (first_player_turn ? "1" : "2") << " has hit a ship!" << endl;
                    continue;
                }
                case GameField::DESTROY_SHIP: {
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

    SimpleRivalBot rival(&game);

    read_player_field(game.field_1());
    rival.place_ships();

    bool player_turn = true;
    while (true) {
        game.print_to_console();

        if (player_turn) while (true) {
            const auto coordinate = read_coordinate_safely(configuration.field_width, configuration.field_height);
            const auto attack_status = game.field_2()->attack(coordinate);
            switch (attack_status) {
                case GameField::ALREADY_ATTACKED: {
                    cout << "> This point has already been attacked" << endl;
                    continue;
                }
                case GameField::MISS: {
                    cout << "> You've missed" << endl;
                    break;
                }
                case GameField::DAMAGE_SHIP: {
                    cout << "> You've hit a ship!" << endl;
                    continue;
                }
                case GameField::DESTROY_SHIP: {
                    cout << "> You've destroyed a ship!" << endl;
                    continue;
                }
                case GameField::WIN: {
                    cout << "> You have won this game!" << endl;
                    return true;
                }
                default: throw invalid_argument("Unknown player-attack status");
            }
            break;
        } else if (rival.act()) return true;

        player_turn = !player_turn;
    }
}

int main() {
    cli::print_logo();

    while (true) {
        cout << "Wanna play?" << endl << endl
                << "Enter `(p)layer` to play against the player, `(b)ot` to play against the bot"
                   " or `(e)xit` or `(q)uit`to exit" << endl;
        string input;
        cin >> input;
        if (input == "p" || input == "player") {
            if (play_against_real_rival()) cli::print_win_message();
            else cli::print_loose_message();
        }
        else if (input == "b" || input == "bot") {
            if (play_against_bot_rival()) cli::print_win_message();
            else cli::print_loose_message();
        }
        else if (input == "e" || input == "q" || input == "exit" || input == "quit") return 0;
    }
}
