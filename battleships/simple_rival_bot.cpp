#include "simple_rival_bot.h"

#include "container_util.h"
#include "game_field.h"

using std::invalid_argument;
using std::runtime_error;
using common_util::not_contains;
using common_util::get_random;

namespace battleships  {

    void SimpleRivalBot::place_ship_randomly(const size_t &ship_size) {
        const auto field = game_->field_2();
        auto original_coordinate = random_coordinate();
        locate_not_visited_spot(original_coordinate, field);

        auto direction = random_direction(random_);
        const auto width = field->get_configuration().field_width, height = field->get_configuration().field_height;
        for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
            const auto tested_coordinate
                    = Coordinate((original_coordinate.x + x) % width, (original_coordinate.y + y) % height);
            for (int i = 0; i < 4; ++i) {
                if (field->try_emplace_ship(tested_coordinate, direction, ship_size)) return;
                direction = rotate_direction_counter_clockwise(direction);
            }
        }

        throw runtime_error("Unable to place s ship at the field");
    }

    void SimpleRivalBot::place_ships() {
        const auto ship_counts = game_->configuration().ship_counts;
        for (auto iterator = ship_counts.rbegin(); iterator != ship_counts.rend(); ++iterator) {
            const auto entry = *iterator;
            for (size_t i = 0; i < entry.second; ++i) place_ship_randomly(entry.first);
        }
    }

    bool SimpleRivalBot::act() {
        return attacked_ship_coordinate_ ? continue_attack() : random_attack();
    }

    bool SimpleRivalBot::continue_attack() {
        const auto initial_coordinate = attacked_ship_coordinate_.value();

        const auto field = game_->field_1();

        //  attempt an attack to reveal the direction
        if (ship_direction_ == NONE) {
            const auto attack_direction
                    = random_available_attack_direction(attacked_ship_coordinate_.value());
            switch (field->attack(initial_coordinate.move(attack_direction, 1))) {
                case GameField::ALREADY_ATTACKED: throw runtime_error("Attempt to attack an already attacked point");
                case GameField::MISS: return false;
                case GameField::DESTROY_SHIP: {
                    handle_ship_destruction();
                    return random_attack();
                }
                case GameField::WIN: return true;
                case GameField::DAMAGE_SHIP: {
                    ship_direction_ = is_horizontal_direction(attack_direction) ? HORIZONTAL : VERTICAL;
                    break;
                }
            }
        }

        // attempt to attack a ship according to its axis
        const auto attack_direction = ship_direction_ == VERTICAL
                ? random_vertical_direction(random_) : random_horizontal_direction(random_);

        auto attacked_coordinate = initial_coordinate.move(attack_direction, 1);
        {
            bool direction_inverted = false;
            while (true) {
                if (field->is_out_of_bounds(attacked_coordinate)) {
                    if (direction_inverted) throw runtime_error("Could not find an appropriate point to attack");
                    else {
                        attacked_coordinate = initial_coordinate.move(attack_direction, -1);
                        direction_inverted = true;
                    }
                }
                switch (field->attack(attacked_coordinate)) {
                    case GameField::ALREADY_ATTACKED: {
                        if (direction_inverted) throw runtime_error("Could not find an appropriate point to attack");
                        else {
                            attacked_coordinate = initial_coordinate.move(attack_direction, -1);
                            direction_inverted = true;
                        }
                        break;
                    }
                    case GameField::MISS: return false;
                    case GameField::DAMAGE_SHIP: break; // simply continue the attack in this direction
                    case GameField::DESTROY_SHIP: {
                        handle_ship_destruction();
                        return random_attack();
                    }
                    case GameField::WIN: return true;
                }

                attacked_coordinate.move(attack_direction, direction_inverted ? 1 : -1);
            }
        }
    }

    bool SimpleRivalBot::random_attack() {
        while (true) {
            auto attacked_coordinate = random_coordinate();
            locate_not_visited_spot(attacked_coordinate, game_->field_1());

            const auto attack_result = game_->field_1()->attack(attacked_coordinate);
            switch (attack_result) {
                case GameField::ALREADY_ATTACKED: throw runtime_error("Cell was expected to not be visited");
                case GameField::MISS: return GameField::MISS; // just missed
                case GameField::DAMAGE_SHIP: {
                    // Multi-celled ship
                    attacked_ship_coordinate_ = attacked_coordinate;
                    continue_attack();
                }
                /* single-celled ship destruction */
                case GameField::DESTROY_SHIP: continue;
                case GameField::WIN: return true;
            }
        }
    }

    void SimpleRivalBot::handle_ship_destruction() {
        attacked_ship_coordinate_.reset();
        ship_direction_ = NONE;
    }

    Direction SimpleRivalBot::random_available_attack_direction(const Coordinate &coordinate) {
        auto direction = random_direction(random_);

        const auto field = game_->field_1();
        if (field->is_discovered(coordinate.move(direction, 1))) {
            if (field->is_discovered(coordinate.move(direction, -1))) {
                direction = rotate_direction_clockwise(direction);
                if (field->is_discovered(coordinate.move(direction, 1))) {
                    if (field->is_discovered(coordinate.move(direction, -1))) throw runtime_error(
                            "No available attack direction for coordinate " + coordinate.to_string()
                    );
                    return invert_direction(direction);
                }
                return direction;
            }
            return invert_direction(direction);
        }
        return direction;
    }
}
