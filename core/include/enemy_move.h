#pragma once
#include <string>
#include "effect_params.h"

struct EnemyMove {
    enum class MoveType { ATTACK, DEFEND, BUFF, SKIP };

    std::string    move_name;
    MoveType       move_type   = MoveType::ATTACK;
    EnemyMoveParams params;
    std::string    intent_icon = "attack";
};
