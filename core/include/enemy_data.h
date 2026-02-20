#pragma once
#include <string>
#include <vector>
#include "enemy_move.h"

struct EnemyData {
    std::string            id;
    std::string            enemy_name;
    int                    max_hp           = 0;
    std::vector<EnemyMove> move_pattern;
    int                    first_move_index = -1;  // -1: 순환, 0+: 첫 턴 고정 무브
};
