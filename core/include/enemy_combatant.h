#pragma once
#include <functional>
#include "combatant.h"
#include "enemy_data.h"

class EnemyCombatant : public Combatant {
public:
    std::function<void(const EnemyMove* move)> on_intent_changed;

    void setup_from_data(const EnemyData* data);
    const EnemyMove* get_current_move() const;
    const EnemyMove* execute_move();   // 현재 무브 반환 후 인덱스 전진
    void start_turn();                 // reset_block + tick_poison

private:
    const EnemyData* _data        = nullptr;
    int              _move_index  = 0;
    int              _turn_count  = 0;

    void _advance_move();
};
