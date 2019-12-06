#ifndef ALGORITHMIC_LANGUAGES_PROJECT_2_GAME_FIELD_H
#define ALGORITHMIC_LANGUAGES_PROJECT_2_GAME_FIELD_H

#include "console_printable.h"
#include <stdexcept>

using std::out_of_range;

namespace battleships {
    class GameField : public ConsolePrintable {

    public:

        virtual ~GameField() = default;

        enum AttackStatus {
            ALREADY_ATTACKED, MISSED, HIT, DESTROY, WIN
        };

        virtual void emplace_ship(const size_t &x, const size_t &y) noexcept(false) = 0;

        virtual GameField::AttackStatus attack(const size_t &x, const size_t &y) noexcept(false) = 0;
    };
}


#endif //ALGORITHMIC_LANGUAGES_PROJECT_2_GAME_FIELD_H
