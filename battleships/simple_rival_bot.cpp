#include "simple_rival_bot.h"

#include "container_util.h"
#include "game_field.h"

using std::invalid_argument;
using common_util::not_contains;

namespace battleships  {

    Coordinate SimpleRivalBot::random_coordinate() {
        return Coordinate(x_random_distribution_(random_), y_random_distribution_(random_));
    }

    void SimpleRivalBot::locate_not_visited_spot(Coordinate &coordinate) {
        if (game_->player_field()->is_visited(coordinate)) {
            const auto step = free_spot_lookup_side_random_distribution_(random_) ? 1 : -1;
            size_t step_count = 0;
            while (true) {
                ++step_count;
                for (size_t i = 0; i < step_count; i++) {

                }
            }
        }

    }

    GameField::AttackStatus SimpleRivalBot::act() {
        if (possible_ship_coordinates_.empty()) return random_attack();
        else return continue_attack();
        // TODO respect possible_ship_coordinates_

        auto target = random_coordinate();

        while (true) {
            auto attack_status = game_->player_field()->attack(target);
            switch (attack_status) {
                case GameField::ALREADY_ATTACKED: {
                    // pick a new target
                }
                case GameField::HIT: {
                    // attack smartly
                    possible_ship_coordinates_.insert(target);
                    const auto direction = random_direction(random_);
                }
                case GameField::DESTROY: {
                    target = random_coordinate();

                    break;
                }
                default: return attack_status;
            }
        }
    }

    GameField::AttackStatus SimpleRivalBot::continue_attack() {
        return GameField::MISSED; // fixme
    }

    GameField::AttackStatus SimpleRivalBot::random_attack() {
        while (true) /* random_attack_attempt: */ {
            auto target = random_coordinate();
            locate_not_visited_spot(target);

            const auto attack_result = game_->player_field()->attack(target);
            switch (attack_result) {
                case GameField::ALREADY_ATTACKED: throw runtime_error("Cell was expected to not be visited");
                case GameField::MISSED: return GameField::MISSED; // just missed
                case GameField::HIT: {
                    mark_as_successfully_attacked(target);
                    continue_attack();
                }
                case GameField::DESTROY: {
                    handle_ship_destruction();
                    continue; // the game has not ended but the attack should be random again
                }
                case GameField::WIN: {
                    handle_ship_destruction();
                    return GameField::WIN;
                }
            }
        }
    }

    void SimpleRivalBot::mark_as_successfully_attacked(const Coordinate &coordinate) {
        visited_possible_ship_coordinates_.insert(coordinate);
        for (const auto &direction : ALL_DIRECTIONS) {
            const auto tested_coordinate = coordinate.move(direction, 1);

            if (not_contains(visited_possible_ship_coordinates_, tested_coordinate)) possible_ship_coordinates_
                    .insert(tested_coordinate);
        }
    }

    void SimpleRivalBot::mark_as_unsuccessfully_attacked(const Coordinate &coordinate) {
        visited_possible_ship_coordinates_.insert(coordinate);
    }

    void SimpleRivalBot::handle_ship_destruction() {
        // cleanup related containers
        possible_ship_coordinates_.clear();
        visited_possible_ship_coordinates_.clear();
    }
}
