#include "enemy_combatant.h"

void EnemyCombatant::setup_from_data(const EnemyData* data) {
    if (!data) return;
    _data = data;
    _turn_count = 0;
    setup(data->enemy_name, data->max_hp);

    // first_move_index >= 0 이면 첫 번째 턴에 해당 무브를 사용
    _move_index = (data->first_move_index >= 0) ? data->first_move_index : 0;
    if (on_intent_changed && !data->move_pattern.empty()) {
        on_intent_changed(&data->move_pattern[_move_index]);
    }
}

const EnemyMove* EnemyCombatant::get_current_move() const {
    if (!_data || _data->move_pattern.empty()) return nullptr;
    return &_data->move_pattern[_move_index];
}

const EnemyMove* EnemyCombatant::execute_move() {
    const EnemyMove* move = get_current_move();
    _advance_move();
    _turn_count++;
    return move;
}

void EnemyCombatant::_advance_move() {
    if (!_data || _data->move_pattern.empty()) return;

    // first_move_index: 첫 번째 턴 이후에는 0번으로 되돌아가는 특수 Cultist 패턴
    if (_data->first_move_index >= 0 && _turn_count == 0) {
        _move_index = 0;
    } else {
        _move_index = (_move_index + 1) % static_cast<int>(_data->move_pattern.size());
    }

    if (on_intent_changed) {
        on_intent_changed(&_data->move_pattern[_move_index]);
    }
}

void EnemyCombatant::start_turn() {
    reset_block();
    tick_poison_start_of_turn();
}
