#pragma once

#include <random>
#include <set>
#include <optional>

#include "rival_bot.h"
#include "direction.h"
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

        GameField *const own_field_, *const rival_field_;

        // Initialized members

        optional<Coordinate> attacked_ship_coordinate_;

        ShipPosition ship_direction_ = NONE;

        random_device random_;


        /* non-const */ bernoulli_distribution free_spot_lookup_side_random_distribution_;
        /* non-const */ uniform_int_distribution<size_t> own_x_random_distribution_, own_y_random_distribution_,
                rival_x_random_distribution_, rival_y_random_distribution_;
        /* non-const */ uniform_int_distribution<int8_t> direction_random_distribution_;

        inline Coordinate random_own_coordinate() {
            return Coordinate(own_x_random_distribution_(random_), own_y_random_distribution_(random_));
        }

        inline Coordinate random_rival_coordinate() {
            return Coordinate(rival_x_random_distribution_(random_), rival_y_random_distribution_(random_));
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

        explicit SimpleRivalBot(GameField *const own_field, GameField *const rival_field)
                : own_field_(own_field), rival_field_(rival_field),
                  own_x_random_distribution_(0, own_field_->get_configuration().field_width - 1),
                  own_y_random_distribution_(0, own_field_->get_configuration().field_height - 1),
                  rival_x_random_distribution_(0, rival_field_->get_configuration().field_width - 1),
                  rival_y_random_distribution_(0, rival_field_->get_configuration().field_height - 1),
                  direction_random_distribution_(0, 3) {}

        void place_ships() override;

        bool act() override;
    };
}


