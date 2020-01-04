#pragma once

#include "game.h"
#include "simple_game_field.h"

namespace battleships {
    class SimpleGame : public Game {

        const GameConfiguration configuration_;
        GameField *field_1_, *field_2_;

    public:

        explicit SimpleGame(const GameConfiguration &configuration) :
                configuration_(configuration),
                field_1_(new SimpleGameField(configuration)), field_2_(new SimpleGameField(configuration)) {}

        ~SimpleGame() override {
            delete field_1_;
            delete field_2_;
        }

        GameConfiguration configuration() override {
            return configuration_;
        }

        GameField *field_1() override {
            return field_1_;
        }

        GameField *field_2() override {
            return field_2_;
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
            cout << "\n";

            {
                auto number = 0;
                for (size_t y = 0; y < height; y++) {
                    cout << number << '|';
                    for (size_t x = 0; x < width; x++) cout
                            << field_1_->get_public_icon_at(Coordinate(x, y)) << '|';
                    cout << "   " << number++ << '|';
                    for (size_t x = 0; x < width; x++) cout
                            << field_2_->get_public_icon_at(Coordinate(x, y)) << '|';
                    cout << "\n";
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
