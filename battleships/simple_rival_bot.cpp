#include "simple_rival_bot.h"

#include "container_util.h"
#include "game_field.h"

using std::invalid_argument;
using std::runtime_error;
using std::to_string;
using common_util::not_contains;
using common_util::get_random;

namespace battleships  {

    void SimpleRivalBot::place_ship_randomly(const size_t &ship_size) {
        auto original_coordinate = random_own_coordinate();
        locate_not_visited_spot(original_coordinate, own_field_);

        const auto width = own_field_->get_configuration().field_width,
                height = own_field_->get_configuration().field_height;

        auto direction = random_direction(random_);
        for (int deltaX = 0; deltaX < width; ++deltaX) for (int deltaY = 0; deltaY < height; ++deltaY) {
            const auto tested_coordinate = Coordinate(
                    (original_coordinate.x + deltaX) % int(width), (original_coordinate.y + deltaY) % int(height)
            );
            for (int i = 0; i < 4; ++i) {
                if (own_field_->try_emplace_ship(tested_coordinate, direction, ship_size)) return;
                direction = rotate_direction_counter_clockwise(direction);
            }
        }

        throw runtime_error("Unable to place " + to_string(ship_size) + "-celled ship at the field");
    }

    void SimpleRivalBot::place_ships() {
        const auto ships = own_field_->get_configuration().ships;
        for (auto iterator = ships.rbegin(); iterator != ships.rend(); ++iterator) {
            const auto entry = *iterator;
            for (size_t shipId = 0; shipId < entry.second; ++shipId) place_ship_randomly(entry.first);
        }
    }

    bool SimpleRivalBot::act(AttackCallback *attack_callback) {
        return attacked_ship_coordinate_.has_value()
                ? continue_attack(EmptyAttackCallback::or_empty(attack_callback))
                : random_attack(EmptyAttackCallback::or_empty(attack_callback));
    }

    bool SimpleRivalBot::continue_attack(AttackCallback *const attack_callback) {
        const auto initial_coordinate = attacked_ship_coordinate_.value();

        //  attempt an attack to reveal the direction
        if (ship_direction_ == NONE) {
            const auto attack_direction = random_available_attack_direction(initial_coordinate);
            const auto attacked_coordinate = initial_coordinate.move(attack_direction, 1);

            const auto attack_status = rival_field_->attack(attacked_coordinate);
            attack_callback->on_attack(attacked_coordinate, attack_status);
            switch (attack_status) {
                case GameField::EMPTY_ALREADY_ATTACKED: case GameField::SHIP_ALREADY_ATTACKED: throw runtime_error(
                        "Attempt to attack an already attacked point"
                );
                case GameField::MISS: return false;
                case GameField::DESTROY_SHIP: {
                    handle_ship_destruction();
                    return random_attack(attack_callback);
                }
                case GameField::WIN: {
                    handle_ship_destruction();
                    return true;
                }
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

        bool direction_inverted = false;
        while (true) {

            if (rival_field_->is_out_of_bounds(attacked_coordinate)) {
                if (direction_inverted) throw runtime_error("Could not find an appropriate point to attack");

                attacked_coordinate = initial_coordinate.move(attack_direction, -1);
                direction_inverted = true;
            }

            const auto attack_status = rival_field_->attack(attacked_coordinate);
            switch (attack_status) {
                case GameField::EMPTY_ALREADY_ATTACKED: {
                    if (direction_inverted) throw runtime_error("Could not find an appropriate point to attack");

                    attacked_coordinate = initial_coordinate; // this will get moved later
                    direction_inverted = true;
                    break;
                }
                case GameField::SHIP_ALREADY_ATTACKED: break; // continue as it is fine to meet the attacked ship point
                case GameField::MISS: {
                    attack_callback->on_attack(attacked_coordinate, GameField::MISS);
                    return false;
                }
                case GameField::DAMAGE_SHIP: {
                    attack_callback->on_attack(attacked_coordinate, GameField::DAMAGE_SHIP);
                    break; // simply continue the attack in this direction
                }
                case GameField::DESTROY_SHIP: {
                    attack_callback->on_attack(attacked_coordinate, GameField::DESTROY_SHIP);
                    handle_ship_destruction();
                    return random_attack(attack_callback);
                }
                case GameField::WIN: {
                    attack_callback->on_attack(attacked_coordinate, GameField::WIN);
                    handle_ship_destruction();
                    return true;
                }
            }

            attacked_coordinate.move(attack_direction, direction_inverted ? -1 : 1);
        }
    }

    bool SimpleRivalBot::random_attack(AttackCallback *const attack_callback) {
        while (true) {
            auto attacked_coordinate = random_own_coordinate();
            locate_not_visited_spot(attacked_coordinate, rival_field_);

            const auto attack_status = rival_field_->attack(attacked_coordinate);
            attack_callback->on_attack(attacked_coordinate, attack_status);
            switch (attack_status) {
                case GameField::EMPTY_ALREADY_ATTACKED: case GameField::SHIP_ALREADY_ATTACKED: throw runtime_error(
                        "Cell was expected to not be visited"
                );
                case GameField::MISS: return false; // just missed
                case GameField::DAMAGE_SHIP: {
                    // Multi-celled ship
                    attacked_ship_coordinate_ = attacked_coordinate;
                    continue_attack(attack_callback);
                }
                /* single-celled ship destruction */
                case GameField::DESTROY_SHIP: continue;
                case GameField::WIN: {
                    handle_ship_destruction();
                    return true;
                }
            }
        }
    }

    void SimpleRivalBot::handle_ship_destruction() {
        attacked_ship_coordinate_.reset();
        ship_direction_ = NONE;
    }

    Direction SimpleRivalBot::random_available_attack_direction(const Coordinate &coordinate) {
        auto direction = random_direction(random_);

        if (rival_field_
                ->can_be_attacked(coordinate.move(direction, 1))) return direction;
        if (rival_field_
                ->can_be_attacked(coordinate.move(direction, -1))) return invert_direction(direction);

        direction = rotate_direction_clockwise(direction);
        if (rival_field_
                ->can_be_attacked(coordinate.move(direction, 1))) return direction;
        if (rival_field_
                ->can_be_attacked(coordinate.move(direction, -1))) return invert_direction(direction);

        throw runtime_error("No available attack direction for coordinate " + coordinate.to_string());
    }
}
