#pragma once

namespace battleships {
    class Game {

    public:
        enum AttackStatus {
            ALREADY_ATTACKED, MISSED, HIT, DESTROY, WIN
        };
    };
}
