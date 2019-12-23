#include "simple_rival_bot.h"

#include "container_util.h"
#include "game_field.h"

using std::invalid_argument;
using common_util::not_contains;
using common_util::get_random;

namespace battleships  {

    GameField::AttackStatus SimpleRivalBot::act() {
        if (possible_ship_coordinates_.empty()) return random_attack();
        else return continue_attack();
    }

    GameField::AttackStatus SimpleRivalBot::continue_attack() {
        /*while (true) {
            const auto direction = get_random(possible_ship_coordinates_, random_);

            if (signgam)

                // fixme
        }*/

        // as a temp fix delegate to random attack (yet it may lead to stack overflow)
        return random_attack();
    }

    GameField::AttackStatus SimpleRivalBot::random_attack() {
        while (true) {
            auto target = random_coordinate();
            locate_not_visited_spot(target);

            const auto attack_result = game_->field_1()->attack(target);
            switch (attack_result) {
                case GameField::ALREADY_ATTACKED: throw runtime_error("Cell was expected to not be visited");
                case GameField::MISSED: return GameField::MISSED; // just missed
                case GameField::HIT: {
                    const auto ship_point = target;
                    for (const auto &direction : ALL_DIRECTIONS) for (size_t delta = 1;
                            delta < game_->configuration().max_ship_length; delta++) {
                        const auto possible_coordinate = ship_point.move(direction, delta);

                        if ((0 <= possible_coordinate.x
                                && possible_coordinate.x < game_->configuration().field_width)
                                && (0 <= possible_coordinate.x
                                && possible_coordinate.x < game_->configuration().field_width)
                        ) possible_ship_coordinates_.insert(possible_coordinate);
                        else break; // inner loop
                    }

                    // FIXME
                    if (true) return continue_attack();
                    switch (continue_attack()) {
                        case GameField::ALREADY_ATTACKED:
                            break;
                        case GameField::MISSED:
                            break;
                        case GameField::HIT:
                            break;
                        case GameField::DESTROY:
                            break;
                        case GameField::WIN:
                            break;
                    }
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

    void SimpleRivalBot::handle_ship_destruction() {
        initial_ship_coordinate_.reset();
        possible_ship_coordinates_.clear();
    }
}
