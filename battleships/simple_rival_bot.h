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

        optional<Coordinate> initial_ship_coordinate_;
        set<Coordinate> possible_ship_coordinates_;
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

        inline void locate_not_visited_spot(/* mut */ Coordinate &coordinate) {
            game_->field_1()->locate_not_visited_spot(
                    coordinate,
                    random_direction(random_),
                    free_spot_lookup_side_random_distribution_(random_)
            );
        }

        GameField::AttackStatus continue_attack();

        GameField::AttackStatus random_attack();

        void handle_ship_destruction();

    public:

        explicit SimpleRivalBot(SimpleGame * const game)
                : game_(game),
                  x_random_distribution_(0, game->configuration().field_width - 1),
                  y_random_distribution_(0, game->configuration().field_height - 1),
                  direction_random_distribution_(0, 3) {}

        GameField::AttackStatus act() override;
    };
}


