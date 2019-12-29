#pragma once

#include <random>
#include <set>
#include <optional>

#include "rival_bot.h"
#include "direction.h"
#include "simple_game.h"
#include "ship_position.h"

using std::default_random_engine;
using std::random_device;
using std::bernoulli_distribution;
using std::uniform_int_distribution;
using std::optional;
using std::set;

namespace battleships {

    class SimpleRivalBot : RivalBot {

    protected:

        // Uninitialized members

        SimpleGame *const game_;

        // Initialized members

        optional<Coordinate> attacked_ship_coordinate_;

        ShipPosition ship_direction_ = NONE;
        /*
        optional<Coordinate> initial_ship_coordinate_;
        set<Coordinate> possible_ship_coordinates_;
         */
        /*
        optional<Coordinate> ship_start_;
        optional<ShipPosition> ship_position_;
        set<Direction> possible_ship_directions_;

        set<Coordinate> possible_ship_coordinates_;
        set<Coordinate> visited_possible_ship_coordinates_;
         */

        random_device random_;


        /* non-const */ bernoulli_distribution free_spot_lookup_side_random_distribution_;
        /* non-const */ uniform_int_distribution<size_t> x_random_distribution_, y_random_distribution_;
        /* non-const */ uniform_int_distribution<int8_t> direction_random_distribution_;

        inline Coordinate random_coordinate() {
            return Coordinate(x_random_distribution_(random_), y_random_distribution_(random_));
        }

        inline void locate_not_visited_spot(/* mut */ Coordinate &coordinate, const GameField *const game_field) {
            game_field->locate_not_visited_spot(
                    coordinate,
                    random_direction(random_),
                    free_spot_lookup_side_random_distribution_(random_)
            );
        }

        void place_ship_randomly(const size_t &ship_size);

        bool continue_attack();

        bool random_attack();

        void handle_ship_destruction();

        Direction random_available_attack_direction(const Coordinate &coordinate);

    public:

        explicit SimpleRivalBot(SimpleGame *const game)
                : game_(game),
                  x_random_distribution_(0, game->configuration().field_width - 1),
                  y_random_distribution_(0, game->configuration().field_height - 1),
                  direction_random_distribution_(0, 3) {}

        void place_ships() override;

        bool act() override;
    };
}


