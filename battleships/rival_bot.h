#pragma once

#include "game_field.h"

namespace battleships {

    /**
     * @brief Bot responsible for playing against the player
     */
    class RivalBot {

    public:

        /**
         * @brief Callback notified on attacks performed by the bot
         */
        class AttackCallback {

        public:
            virtual void on_attack(const Coordinate &coordinate, const GameField::AttackStatus &attack_status) = 0;
        };

        /**
         * @brief Attack callback doing nothing
         */
        class EmptyAttackCallback : public AttackCallback {

        public:

            [[nodiscard]] static AttackCallback *empty() {
                /**
                 * @brief Singleton instance of this empty attack callback
                 */
                static EmptyAttackCallback INSTANCE;
                return &INSTANCE;
            }

            [[nodiscard]] static AttackCallback *or_empty(AttackCallback *const attack_callback) {
                return attack_callback ? attack_callback : empty();
            }

            void on_attack(const Coordinate &coordinate, const GameField::AttackStatus &attack_status) override {};
        };

        virtual void place_ships() = 0;

        virtual bool act(AttackCallback *attack_callback) = 0;
    };
}


