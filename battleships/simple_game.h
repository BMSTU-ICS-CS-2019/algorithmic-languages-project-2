#pragma once

#include "game.h"
#include "simple_game_field.h"

namespace battleships {
    class SimpleGame : public Game {

        const GameConfiguration configuration_;
        SimpleGameField player_field_, bot_field_;

    public:

        explicit SimpleGame(const GameConfiguration &configuration) :
                configuration_(configuration), player_field_(configuration), bot_field_(configuration) {}

        GameConfiguration configuration() override {
            return configuration_;
        }

        GameField *player_field() override {
            return &player_field_;
        }

        GameField *bot_field() override {
            return &bot_field_;
        }

        void print_to_console() const noexcept override {
            const auto width = configuration_.field_width, height = configuration_.field_height;
            // draw upper border
            {
                cout << ' ';
                auto letter = 'A';
                for (size_t i = 0; i < width * 2 + 1; i++) cout << (i % 2 == 0 ? '|' : letter++);
            }
            cout << "   ";
            {
                cout << ' ';
                auto letter = 'A';
                for (size_t i = 0; i < width * 2 + 1; i++) cout << (i % 2 == 0 ? '|' : letter++);
            }
            cout << endl;

            {
                auto number = 0;
                for (size_t y = 0; y < height; y++) {
                    cout << number << '|';
                    for (size_t x = 0; x < width; x++) cout << player_field_.get_icon_at(Coordinate(x, y)) << '|';
                    cout << "   " << number++ << '|';
                    for (size_t x = 0; x < width; x++) cout << bot_field_.get_icon_at(Coordinate(x, y)) << '|';
                    cout << endl;
                }
            }

            // draw lower border
            cout << ' ';
            for (size_t i = 0; i < width * 2 + 1; i++) cout << "¯";
            cout << "    ";
            for (size_t i = 0; i < width * 2 + 1; i++) cout << "¯";
            cout << endl;
        }
    };
}
